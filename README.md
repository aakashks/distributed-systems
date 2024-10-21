# Birman-Schiper-Stephenson Protocol

## Running the code

Must have gcc installed to compile the code. Linux / Macos specific instructions are given below.

Run the following command to compile the code

```bash
g++ -std=c++11 bss.cpp -o bss
```

To run the code for an input file (eg. [input](input)) -
```bash
./bss < input
```

To save the output in a file, run the following command -
```bash
./bss < input > output
```

there are 3 input files in the repository, you can use them to test the code.

## Sample Inputs

Input 1:
```
begin process p1
send m1
recv_B p2 m2
end process p1

begin process p2
recv_B p1 m1
send m2
end process p2

begin process p3
recv_B p2 m2
recv_B p1 m1
end process p3
```

Output 1:
```
begin process p1
send m1 (100)
recv_B p2 m2 (100)
recv_A p2 m2 (110)
end process p1

begin process p2
recv_B p1 m1 (000)
recv_A p1 m1 (100)
send m2 (110)
end process p2

begin process p3
recv_B p2 m2 (000)
recv_B p1 m1 (000)
recv_A p1 m1 (100)
recv_A p2 m2 (110)
end process p3

```

Input 2:
```
begin process p2
send m2
recv_B p4 m4
recv_B p1 m1
end process p2

begin process p1
send m1
recv_B p4 m4
recv_B p2 m2
end process p1

begin process p4
recv_B p2 m2
recv_B p1 m1
send m4
end process p4

begin process p3
recv_B p2 m2
recv_B p1 m1
recv_B p4 m4
end process p3
```

Output 2:
```
begin process p2 
send m2 (0100) 
recv_B p4 m4 (0100) 
recv_B p1 m1 (0100) 
recv_A p1 m1 (1100) 
recv_A p4 m4 (1101) 
end process p2 

begin process p1 
send m1 (1000) 
recv_B p4 m4 (1000) 
recv_B p2 m2 (1000) 
recv_A p2 m2 (1100) 
recv_A p4 m4 (1101) 
end process p1 

begin process p4 
recv_B p2 m2 (0000) 
recv_A p2 m2 (0100) 
recv_B p1 m1 (0100) 
recv_A p1 m1 (1100) 
send m4 (1101) 
end process p4 

begin process p3 
recv_B p2 m2 (0000) 
recv_A p2 m2 (0100) 
recv_B p1 m1 (0100) 
recv_A p1 m1 (1100) 
recv_B p4 m4 (1100) 
recv_A p4 m4 (1101) 
end process p3

```
