Commands to control the device via UART
Lines starting with '##' are the output from the device,
an answer on user actions or control commands


## 1. Power control


**amp.on**                      - Power on  
  ##AMP.STATUS#: ACTIVE  
  ##AMP.AUDIO.IC#: TDA7439  
  ##AMP.AUDIO.INPUT#: 0  
  ##AMP.AUDIO.VOLUME#: -50  
  ##AMP.AUDIO.BASS#: 3  
  ##AMP.AUDIO.MIDDLE#: 1  
  ##AMP.AUDIO.TREBLE#: 2  
  ##AMP.AUDIO.FRONTREAR#: 0  
  ##AMP.AUDIO.BALANCE#: 0  
  ##AMP.AUDIO.CENTER#: 0  
  ##AMP.AUDIO.SUBWOOFER#: 0  
  ##AMP.AUDIO.PREAMP#: 0  

**amp.off**                     - Power off  
  ##AMP.STATUS#: STANDBY  

**amp.switch**                  - Toggle the power  
  ##AMP.STATUS#: STANDBY  

**amp.status**                  - Show current power status  
  ##AMP.STATUS#: ACTIVE  

Commands above work always. Commands below work only in ACTIVE status


## 2. Audio control

### Inputs

**amp.input**                   - Show current input  
 ##AMP.AUDIO.INPUT#: 2  

**amp.input 1**                 - Select input (starting from 0)  
  ##AMP.AUDIO.INPUT#: 1  
  ##AMP.AUDIO.GAIN#: 0  

**amp.input.next**              - Switch to next input  
  ##AMP.AUDIO.INPUT#: 3  
  ##AMP.AUDIO.GAIN#: 0  

**amp.input.prev**              - Switch to previous input  
  ##AMP.AUDIO.INPUT#: 2  
  ##AMP.AUDIO.GAIN#: 0  

### Volume, bass, middle. treble:

**amp.volume**                  - Show volume level  
  ##AMP.AUDIO.VOLUME#: -53  

**amp.bass.inc**                - Increase bass level  
  ##AMP.AUDIO.BASS#: 4  

**amp.middle.dec**              - Decrease middle level  
  ##AMP.AUDIO.MIDDLE#: 0  

**amp.treble 3**              - Decrease treble level  
  ##AMP.AUDIO.TREBLE#: 3  
