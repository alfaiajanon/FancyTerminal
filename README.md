# FancyTerminal
A lightweight, cross-platform terminal UI (TUI) library written entirely in C.
While many TUI libraries exist, few are built from the ground up in pure C (and cross-platform) — FancyTerminal fills that gap with simplicity and full control.

<br>


# Usage

- First initialize a fancy terminal container
    
    ```c    
    FancyTerminal *ft=initFancyTerminal();
    ```
- Configure the layout (see [User Guide](./docs/UserGuide.md) for more details)
    
    ```c
    setFTLayoutPattern(
    ft,
    2,5,
    "1 1 0 2 2,"
    "1 1 0 2 2,"
    );
    ```
- Create UI elements and add them 

    ```c
    FTElement *button=createFTButton("Button");                            
    addToFancyTerminal(ft,button);

    FTElement *textField=createFTTextField("Text Hint","");    
    addToFancyTerminal(ft,text_field);
    ```
- Register event handlers (optional)
    
    ```c
    ft_connect(move_btn, FT_EVENT_CLICK, myFunction, NULL);
    ```
- Finally, Start the TUI loop

    ```c
    enterFancyTerminal(ft);
    ```
<br>


# Demo 

![Demo Image](demo.png)

Building the Demo

```bash
git clone https://github.com/alfaiajanon/FancyTerminal.git
cd FancyTerminal
cd build
cmake .
make
```
<br>


# External Library

[rogueutil](https://github.com/sakhmatd/rogueutil) is used to bring cross platform support


# Documentation

- [User Guide](./docs/UserGuide.md)
<!-- - [Developer Guide](./docs/DeveloperGuide.md) -->