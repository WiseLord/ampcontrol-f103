Commands to control the device via UART
Lines starting with '##' are the output from the device,
an answer on user actions or control commands


## 1. Power control


**amp.on**                      - Power on  
  ##AMP.STATUS#: ACTIVE  

**amp.off**                     - Power off  
  ##AMP.STATUS#: STANDBY  

**amp.switch**                  - Toggle the power  
  ##AMP.STATUS#: STANDBY  

**amp.status**                  - Show current power status  
  ##AMP.STATUS#: ACTIVE  

Commands above work always. Commands below work only in ACTIVE status


## 2. Audio input control


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

  
## 3. Audio parameters control 


### Volume:

**amp.volume**                  - Show volume level  
  ##AMP.AUDIO.VOLUME#: -53  

**amp.volume.inc**              - Increase volume level  
  ##AMP.AUDIO.VOLUME#: -52  

**amp.volume.dec**              - Decrease volume level  
  ##AMP.AUDIO.VOLUME#: -53  

### Bass:

**amp.bass**                    - Show bass level  
  ##AMP.AUDIO.BASS#: 3  

**amp.bass.inc**                - Increase bass level  
  ##AMP.AUDIO.BASS#: 4  

**amp.bass.dec**                - Decrease bass level  
  ##AMP.AUDIO.BASS#: 3  

### Middle:

**amp.middle**                  - Show freq level  
  ##AMP.AUDIO.MIDDLE#: 0  

**amp.middle.inc**              - Increase middle level  
  ##AMP.AUDIO.MIDDLE#: 1  

**amp.middle.dec**              - Decrease middle level  
  ##AMP.AUDIO.MIDDLE#: 0  

### Treble:

**amp.treble**                  - Show treble level  
  ##AMP.AUDIO.TREBLE#: 0  

**amp.treble.inc**              - Increase treble level  
  ##AMP.AUDIO.TREBLE#: 1  

**amp.treble.dec**              - Decrease treble level  
  ##AMP.AUDIO.TREBLE#: 0  
