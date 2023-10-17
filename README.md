# sd-get-prompt

Easy display for Stable Diffusion iTXt Exif data. Anyone can copy and paste friom dialog.

Sample picture is Japanese language but everybodは can understanding through SD(Stable Diffusion) icon picture on right click menu.

生成AI設定情報 → Creation AI Configuration Info.

# Usage (Right Clickable)

How to use?

1. Add path to ~/bin and place it.

## KDE

1. create .desktop file.
2. place to .kde/share/kde4/services/ServiceMenus/

[Desktop Entry]  
Version=1.0  
Type=Application  
Name=sd-get-prompt  
Comment=Get tEXt parametor  
Exec=~/bin/sd-get-prompt %u  
ServiceTypes=KonqPopupMenu/Plugin  
MimeType=image/png  
Icon=applications-graphics  
Path=  
Terminal=false  
StartupNotify=true  

& Apply to KDE Dolphin service dir and good changes.

## XFce

1. Add right-click action for thunar.

## Others

for Other wm(window Manager) and file manager.

1. Should be reading your file manager manpages.
