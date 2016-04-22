2a# Note: configuration file in /etc/xneur is just a template. On first run XNeur\n# copies it into user's .xneur/. So all changes in this file will affect only\n# new users, who didn't run XNeur before.
s/^LogLevel Debug/LogLevel Error/
s/^AddAction/#AddAction/
s/^LogSave Yes/LogSave No/
s#/usr/local/#/usr/#g
s/XneurStart Enable .*/XneurStart Enable X Neural Switcher started/
s/XneurReload Enable .*/XneurReload Enable X Neural Switcher restarted/
s/XneurStop Enable .*/XneurStop Enable X Neural Switcher stopped/
s/PreviewChangeSelected .*/PreviewChangeSelected Enable Preview selected text correction/
s/PreviewChangeClipboard .*/PreviewChangeClipboard Enable Priview clipboard text correction/
s/ExecuteUserAction Enable .*/ExecuteUserAction Enable Running command/
