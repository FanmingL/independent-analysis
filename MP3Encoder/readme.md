IVA Implementation On DSP Board
===
Independent Vector Analysis algorithm has been implemented on DEC6713 DSP Board, whoes close frequency is up to 300MHz.

About the css project
---
This project is developed on **CSS5.5** platform with a JTAG named **SEED-XDS510PLUS**(the took kit perform so bad, it takes about **90s** to
burn the program to RAM, and **1.5 hours** to burn the program to FLASH!). I spend almost two weeks to set up and suit the awful development environment.
<br>
<br>
come back to the ultimate topic. This project is based on a demo, in which almost all codes have been packing(in the other word, no source code), 
provided by SEED company. After my adding the iva code into the project, the code executing effect is too limited to produce good results.
There are some tips I used to improve the effect.

Tips
---
- Use the L2 memory as much as possible, I put almost all the code and data to the L2 memory, especially the **heap**, **stack**, **.test**, **.bios**, **.bss**.
- Prevent from frequently using the function like, **sin**, **cos**, **malloc/free**, and even the **divide** operation. For example, the FFT function, there are 
calculations of cos and sin, which cost a lot of time, considering this project, the FFT points is fixed, so I calculate all the sin and cos value that could be used at 
the initialization period, and put them in a table. This action is helpful.
- In the demo program, the clock frequncy is set at 225MHz, we can simple set the **PLLM** register to 24 to elevate the clock frequency to 300MHz.


About the flash burn
---
To realize the second bootloader, we should
- keep out the 0x00000000 to 0x000003ff, where the first bootloader will move the second bootloader to.
- write the second bootloader program, which can be find in google.
- use hex6x tool to generate the ascii file.
- use flashburn tool to burn to hex file into the board's flash. 
