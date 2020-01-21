# about remotePulse

## raspberry pi 4 setup

### scheduled reboot 
Create a file named "mycron" (or whatever your like) in the `/etc/cron.d/` folder.
```
sudo nano /etc/cron.d/mycron

# past in to this file:
# m h dom mon dow user command
9 21 * * * root /sbin/shutdown -r now
```

### auto start app after desktop appears
```
sudo nano /etc/xdg/lxsession/LXDE-pi/autostart


#this line makes this app start 
sudo $HOME/Desktop/cardinal_pi

sudo /home/pi/openFrameworks/apps/remotePulse/remotePulse/bin/remotePulse
```

###  Adding A New Desktop Shortcut

Create a new text file called yourname.desktop in the /home/pi/Desktop/ directory, e.g. /home/pi/Desktop/yourname.desktop

It should contain the following:

```
[Desktop Entry]
Name=remotePulse
Comment=My application which does this
Icon=/home/pi/openFrameworks/apps/remotePulse/remotePulse/bin/icon.xpm
Exec=sudo /home/pi/openFrameworks/apps/remotePulse/remotePulse/bin/remotePulse
Type=Application
Encoding=UTF-8
Terminal=false
Categories=None;
```

### updating realVNC
sudo apt-get install realvnc-vnc-server


## troubleshooting

### File Manager/Desktop folders instantly closes when opened. 
Reinstalling pcmanfm worked for me:
```
sudo apt-get install --reinstall pcmanfm
```

![Screenshot of emptyExample](emptyExample.png)
