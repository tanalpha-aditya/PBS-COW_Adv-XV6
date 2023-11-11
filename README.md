[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/JH3nieSp)
# OSN Monsoon 2023 mini project 3
## xv6 revisited and concurrency

*when will the pain and suffering end?*

## Some pointers/instructions
- main xv6 source code is present inside `initial_xv6/src` directory.
- Feel free to update this directory and add your code from the previous assignment here.
- By now, I believe you are already well aware on how you can check your xv6 implementations. 
- Just to reiterate, make use of the `procdump` function and the `usertests` and `schedulertest` command.
- work inside the `concurrency/` directory for the Concurrency questions (`Cafe Sim` and `Ice Cream Parlor Sim`).

- Answer all the theoretical/analysis-based questions (for PBS scheduler and the concurrency questions) in a single `md `file.
- You may delete these instructions and add your report before submitting.

Few assumptions 
- for con -q2, i am assuming the input to end when a 'end' will be printed. so whevener you want to end the input taking of program just write 'end'
- I referred to online sources for implementing COW and the major reference and idea i got was from this one chinese website - https://blog.csdn.net/LostUnravel/article/details/121418548 but none of code is copied. everything is written from scratch
- for concurrency questions i have randomized the printing of outputs if they are occuring at the same second ( same moment )
- I have implemented this to the best of my knowledge and any edgecases which i must have forgotten can be implement by me if explcitly told then are there. As this whole codebase is made from scratch by me I know whereabouts of everything i used and any edgecases can be handled quickly. 
