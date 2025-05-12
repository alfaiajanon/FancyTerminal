### Topics

- FancyTerminal Setup
- FT Elements
- FT Event callbacks
- Decorations
<br>


### FancyTerminal Setup

Instance Functions

- `FancyTerminal *ft=initFancyTerminal();`
  Creates a new FancyTerminal instance. 
<br>

- `enterFancyTerminal(ft);`
  Enters the FancyTerminal loop. (Codes after this line will not be executed until you exit the loop)
<br>  

- `exitFancyTerminal(ft);`
  Exits the FancyTerminal loop.
<br>


Layout Functions

- `setFTLayoutPattern(FancyTerminal *ft, int rows, int cols, char *pattern);`
  Sets the layout pattern.
  New row is indicated by `,`.
  Elements will get connected to index based on the order they are added.
  pattern example:
  ```c
  setFTLayoutPattern(
    ft,
    2,3,
    "1 0 2,"
    "1 0 3,"
  )
  ```
  this will create a layout with 2 rows and 3 columns where the first element will be connected to index 1, the second element will be connected to index 2 and so on. The `0` indicates that is a empty space. 
<br>

- `addToFancyTerminal(FancyTerminal *ft, FTElement *element);`
  Adds an element to the FancyTerminal.
<br>



### FT Elements


``` work in progress ```