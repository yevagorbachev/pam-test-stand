# Layout
```
pam-test-stand.ino:     global variables and utilties not placed in ./src/
daq.ino:                data acquisition app
src/<Library>:          library
```

The main `.ino` file is always included. The Arduino CLI concatenates all
`.ino` files, therefore the applicatiion to be compiled is defined by an `APP_<name>`
define directive passed into the `arduino-cli` invocation, and each app will
have a matching `#ifdef` to include or omit its code.
