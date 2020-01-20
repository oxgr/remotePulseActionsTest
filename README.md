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

sudo apt-get install realvnc-vnc-server


## troubleshooting

### File Manager/Desktop folders instantly closes when opened. 
Reinstalling pcmanfm worked for me:
```
sudo apt-get install --reinstall pcmanfm
```

![Screenshot of emptyExample](emptyExample.png)
