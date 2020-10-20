# Binalyze
Binary analysis tool using libbfd and Capstone API version 4.0.2  
(https://www.capstone-engine.org/)

Usage: **./binalyze -f binary [-c section] [-odslr]**

**-c** : prints section information and content in hexadecimal and ASCII format  
**-o** : prints static symbols  
**-d** : prints dynamic symbols  
**-s** : prints sections  
**-l** : disassembles code with linear disassembly  
**-r** : disassembles code with recursive disassembly  
