# PrestoBash

This is an GUI application for systems that use the Unix shell **Bash**.

On opening it parses the `~/.bashrc` file for user added aliases and creates buttons that, when pressed, run the commands that correspond to said aliases.

The user can also create new `.bashrc` aliases, as well as in-app shortcuts that function the same way as `.bashrc` aliases. These shortcuts are saved and ready to be used immediately; they are also read and restored on each application start. The application also offers the ability to edit the `.baschr` and shortcuts files directly from the UI.

The output of each command is displayed in a text browser in the application window.

*Note no.0:* The application in its current state of development doesn't offer the ability for user input if a running command asks for it, except for the case it asks for the root password (in order to execute `sudo` commands). For the password prompt to work, you need to have `ssh-askpass` configured first.

*Note no.1:* The interface will wait for each command to finish its execution. Trying to run something else, or close the application, while a command is running, may cause the interface to freeze. It will un-freeze when the running command finishes.

*Note no.2:* For the time being, multiple one-line commands that use `;` are treated the same way as the ones that use `&&`, i.e. the execution of each command will take place only if the previous one has been executed successfully, as I haven't so far found a way to provide for cases that use both at the same time.


### to compile & install from source:

1. Donwload the code or clone the repo
2. Open extracted directory in terminal and run:

    `qmake tiki.pro CONFIG+=release && make && sudo make install && make clean`

You will need to have basic Qt libraries installed first.
