import getopt
import sys
import threading
import time

import serial
import mpd
from select import select


def do_meta(song):
    title = song.get('title', '')
    name = song.get('name', '')
    artist = song.get('artist', '')

    if title:
        if artist:
            name = artist + ' - ' + title
        else:
            name = title

    return name


class Console(object):
    def __init__(self, port, baudrate):
        self.serial = serial.Serial(port=port, baudrate=baudrate)
        self.alive = False
        self.reader_thread = None
        self.parse_cb = None
        self.write_lock = threading.Lock()

    def set_parse_cb(self, cb):
        self.parse_cb = cb

    def send(self, info):
        self.write_lock.acquire()
        print(">>>: " + info)
        self.serial.write(bytes(info + '\r\n', 'utf-8'))
        self.write_lock.release()

    def start(self):
        self.alive = True
        self.reader_thread = threading.Thread(target=self.reader_fn, name='reader')
        self.reader_thread.daemon = True
        self.reader_thread.start()

    def join(self):
        self.reader_thread.join()

    def reader_fn(self):
        while self.alive:
            try:
                str_seq = self.serial.read_until(serial.LF, 256)
                str_seq = str(str_seq, 'utf-8').strip()
                if self.parse_cb:
                    self.parse_cb(str_seq)
                if str_seq == 'quit':
                    self.alive = False
            except:
                pass


class Player(object):
    def __init__(self, sport, baudrate, host, port):
        self.client = mpd.MPDClient()
        self.console = Console(port=sport, baudrate=baudrate)
        self.console.set_parse_cb(self.parse_handler)
        self.client.connect(host=host, port=port)
        self.alive = False
        self.notify_thread = None
        self.player_info = []
        self.cmd_queue = []

    def parse_cmd(self, cmd, status):
        elapsed = float(status.get('elapsed', 0))
        state = status.get('state', 'stop')

        cmd = str(cmd)

        if cmd == 'info':
            self.player_info = []
        if cmd == 'start':
            self.client.play()
        if cmd == 'stop':
            self.client.stop()
        if cmd == 'pause':
            if state == 'stop':
                self.client.play()
            else:
                self.client.pause()
        if cmd == 'next':
            if state == 'play':
                self.client.next()
        if cmd == 'previous':
            if state == 'play':
                self.client.previous()
        if cmd == 'rewind':
            if state == 'play':
                pos = elapsed - 5
                if pos < 0:
                    pos = 0
            self.client.seekcur(pos)
        if cmd == 'ffwd':
            if state == 'play':
                pos = elapsed + 5
                self.client.seekcur(pos)
        if cmd.startswith('load'):
            playlists = self.client.listplaylists()
            pl_name = cmd[6:-2].strip()
            if next((pl for pl in playlists if pl['playlist'] == pl_name), False):
                self.client.clear()
                self.client.load(pl_name)
                self.client.play()
        if cmd == 'repeat':
            self.client.repeat(int(not int(status.get(cmd))))
        if cmd == 'random':
            self.client.random(int(not int(status.get(cmd))))
        if cmd == 'single':
            self.client.single(int(not int(status.get(cmd))))
        if cmd == 'consume':
            self.client.consume(int(not int(status.get(cmd))))

    def notify_fn(self):
        while self.alive:
            self.update_player_info()
            if not self.cmd_queue:
                time.sleep(0.1)

    def start(self):
        self.alive = True
        self.console.start()
        self.notify_thread = threading.Thread(target=self.notify_fn, name='notify')
        self.notify_thread.daemon = True
        self.notify_thread.start()

    def join(self):
        self.console.join()
        self.notify_thread.join()

    def update_player_info(self):
        status = self.client.status()
        if self.cmd_queue:
            cmd = self.cmd_queue.pop(0)
            self.parse_cmd(cmd, status)
            status = self.client.status()
        song = self.client.currentsong()

        player_info = {
            'name': song.get('name', ''),
            'artist': song.get('artist', ''),
            'title': song.get('title', ''),
            'elapsed': float(status.get('elapsed', 0)),
            'duration': float(status.get('duration', 0)),
            'state': status.get('state'),
            'repeat': status.get('repeat'),
            'random': status.get('random'),
            'single': status.get('single'),
            'consume': status.get('consume'),
            'timestamp': time.time(),
        }

        update_all = False
        if not self.player_info:
            self.player_info = player_info
            update_all = True

        update_name = player_info['name'] != self.player_info['name']
        update_title = player_info['title'] != self.player_info['title']
        update_artist = player_info['artist'] != self.player_info['artist']
        update_meta = update_name or update_title or update_artist

        update_elapsed = (player_info['state'] == 'play' and
                          round(player_info['elapsed']) != round(self.player_info['elapsed']))

        update_duration = player_info['duration'] != self.player_info['duration']

        update_state = player_info['state'] != self.player_info['state']

        update_repeat = player_info['repeat'] != self.player_info['repeat']
        update_random = player_info['random'] != self.player_info['random']
        update_single = player_info['single'] != self.player_info['single']
        update_consume = player_info['consume'] != self.player_info['consume']

        self.player_info = player_info

        if update_all or update_meta:
            self.send_meta()
        if update_all or update_elapsed:
            self.send_elapsed()
        if update_all or update_duration:
            self.send_duration()
        if update_all or update_state:
            self.send_state()
        if update_all or update_repeat:
            self.send_repeat()
        if update_all or update_random:
            self.send_random()
        if update_all or update_single:
            self.send_single()
        if update_all or update_consume:
            self.send_consume()

    def send_meta(self):
        self.console.send('##CLI.META#: ' + do_meta(self.player_info))

    def send_elapsed(self):
        self.console.send('##CLI.ELAPSED#: ' + str(round(self.player_info['elapsed'])))

    def send_duration(self):
        self.console.send('##CLI.DURATION#: ' + str(round(self.player_info['duration'])))

    def send_state(self):
        if self.player_info['state'] == 'play':
            self.console.send('##CLI.PLAYING#')
        elif self.player_info['state'] == 'pause':
            self.console.send('##CLI.PAUSED#')
        else:
            self.console.send('##CLI.STOPPED#')

    def send_repeat(self):
        self.console.send('##CLI.REPEAT#: ' + self.player_info['repeat'])

    def send_random(self):
        self.console.send('##CLI.RANDOM#: ' + self.player_info['random'])

    def send_single(self):
        self.console.send('##CLI.SINGLE#: ' + self.player_info['single'])

    def send_consume(self):
        self.console.send('##CLI.CONSUME#: ' + self.player_info['consume'])

    def parse_handler(self, input):
        input = str(input).strip()
        if input:
            print("<<<: '" + input + "'")
            if input.startswith('cli.'):
                command = input[len('cli.'):]
                self.cmd_queue.append(command)


def main(argv):
    port = '/dev/serial0'
    baudrate = 115200
    try:
        opts, args = getopt.getopt(argv, "p:b:", ["port=", "baudrate="])
    except getopt.GetoptError:
        print("Wrong command line arguments")
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-p", "--port"):
            port = arg
        if opt in ("-b", "--baudrate"):
            baudrate = arg

    player = Player(sport=port, baudrate=baudrate, host="localhost", port=6600)
    player.start()
    player.join()


if __name__ == "__main__":
    main(sys.argv[1:])
