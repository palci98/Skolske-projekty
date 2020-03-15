import xml.dom.minidom
import re
import sys

Noneargs = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]
Labelargs =["LABEL","JUMP","CALL"] #one argument and the argument is Label
Varsargs =["DEFVAR", "POPS"] #one argument and the argument is var
Symbargs = ["DPRINT", "PUSHS", "WRITE", "EXIT", ] #one argument and the argument is Symbol
VarSymbargs=["MOVE", "INT2CHAR", "STRLEN", "NOT", "TYPE"] #two arguments var and symbol
VarTypeargs=["READ"] #two arguments var and type
VarSymSymargs = ["ADD", "SUB", "MUL", "IDIV", "AND", "OR", "GT", "LT", "EQ", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR"] #three arguments var, symbol and symbol
LabelSymSymargs=["JUMPIFNEQ", "JUMPIFEQ"] #three arguments Label, symbol, symbol

XML_file = {} #Global variable where xml will be loaded

def ArgumentChecker(argv,argc):
    if argc == 2:
        if argv[1][:9]=="--source=":
            return {"source":argv[1][9:],"input": None}
        elif argv[1][:8]=="--input=":
            return {"source":sys.stdin,"input": argv[1][8:]}
        elif argv[1][:6]=="--help":
            print("pre spravne fungovanie programu: program.py --source=file --input=file or program.py --source=file <stdin or --input=file <stdin")
            exit(0)
        else:
            exit(10)
    elif argc == 3:
        if argv[1][:9]=="--source=":
            if argv[2][:8]=="--input=":
                return {"source":argv[1][9:],"input": argv[2][8:]}            
            else:
                exit(10)
        if argv[1][:8]=="--input=":
            if argv[2][:9]=="--source=":
                return {"source":argv[2][9:],"input": argv[1][8:]}            
            else:
                exit(10)
    else:
        exit(10)
    
source={}
source=ArgumentChecker(sys.argv,len(sys.argv))

class LoadXML():
    def __init__(self,file):
        self.file = file

    def LoadArgs(self,collection):
        instructions = collection.getElementsByTagName("instruction")
        for instruction in instructions:
            if not (instruction.hasAttribute("order") and instruction.hasAttribute("opcode")):
                exit(31)
            if instruction.hasAttribute("order"):
                count=(instruction.getAttribute("order")) 
                if not count.isdigit():
                    exit(31)
                if instruction.hasAttribute("opcode"):
                    if str(count) in XML_file:
                        exit(32)
                    XML_file[count] = {}
                    argument1=instruction.getElementsByTagName("arg1")
                    argument2=instruction.getElementsByTagName("arg2")
                    argument3=instruction.getElementsByTagName("arg3")
                    if(len(argument1)>1 or len(argument2)>1 or len(argument3)>1):
                            exit(32)                            
                    if instruction.getAttribute("opcode") in Noneargs:
                        if(len(argument1) !=0 or len(argument2)!=0 or len(argument3)!=0):
                            exit(32)
                        XML_file[count]["instruction"] = instruction.getAttribute("opcode")                        
                    elif instruction.getAttribute("opcode") in Labelargs:
                        if(len(argument2)!=0 or len(argument3)!=0):
                            exit(32)                        
                        self.MoreLabelArgs(instruction,count,"arg1")
                    elif instruction.getAttribute("opcode") in Varsargs:
                        if(len(argument2)!=0 or len(argument3)!=0):
                            exit(32)   
                        self.MoreVarsArgs(instruction,count,"arg1")
                    elif instruction.getAttribute("opcode") in Symbargs:
                        if(len(argument2)!=0 or len(argument3)!=0):
                            exit(32)   
                        self.MoreSymbArgs(instruction,count,"arg1")
                    elif instruction.getAttribute("opcode") in VarSymbargs:
                        if(len(argument3)!=0):
                            exit(32)   
                        self.MoreVarsArgs(instruction,count,"arg1")
                        self.MoreSymbArgs(instruction,count,"arg2")
                    elif instruction.getAttribute("opcode") in VarTypeargs:
                        if(len(argument3)!=0):
                            exit(32)
                        self.MoreVarsArgs(instruction,count,"arg1")
                        self.MoreTypeArgs(instruction,count,"arg2")
                    elif instruction.getAttribute("opcode") in VarSymSymargs:
                        self.MoreVarsArgs(instruction,count,"arg1")
                        self.MoreSymbArgs(instruction,count,"arg2")
                        self.MoreSymbArgs(instruction,count,"arg3")
                    elif instruction.getAttribute("opcode") in LabelSymSymargs:
                        self.MoreLabelArgs(instruction,count,"arg1")
                        self.MoreSymbArgs(instruction,count,"arg2")
                        self.MoreSymbArgs(instruction,count,"arg3")
                    else:
                        exit(32)
                else:
                    exit(32)
            else:
                exit(32)

    def AddArgs(self,instruction,count,string,typer):          
        if instruction.hasAttribute("opcode"):
            XML_file[count]["instruction"] = instruction.getAttribute("opcode")
        else:
            exit(32)
        XML_file[count][string] = {}
        if typer[0].hasAttribute("type"): 

            XML_file[count][string]["type"] = typer[0].getAttribute("type")

            if len(typer[0].childNodes)==0:
                exit(32)
            else:    
                XML_file[count][string]["value"] = typer[0].childNodes[0].data 
    
        else:
            exit(32)

    def MoreVarsArgs(self,instruction,count,string):
        if not count in XML_file:
            XML_file[count] = {}
        typer = instruction.getElementsByTagName(string)
        if len(typer) == 0:
            exit(32)
        else:
            if(instruction.getAttribute("opcode")in Varsargs or instruction.getAttribute("opcode")in  VarSymbargs or instruction.getAttribute("opcode")in  VarTypeargs or instruction.getAttribute("opcode")in  VarSymSymargs ):
                if(typer[0].getAttribute("type")=="var"):
                    self.AddArgs(instruction,count,string,typer)
                else:
                    exit(32)
            else:
                exit(32)

    def MoreTypeArgs(self,instruction,count,string):
        if not count in XML_file:
            XML_file[count] = {}
        typer = instruction.getElementsByTagName(string)
        if len(typer) == 0:
            exit(32)
        else:
            if(instruction.getAttribute("opcode")in VarTypeargs):
                if(typer[0].getAttribute("type")=="type"):
                    self.AddArgs(instruction,count,string,typer)
                else:
                    exit(32)
            else:
                exit(32)

    def MoreSymbArgs(self,instruction,count,string):
        if not count in XML_file:
            XML_file[count] = {}
        typer = instruction.getElementsByTagName(string)
        if len(typer) == 0:
            exit(32)
        else:
            if(instruction.getAttribute("opcode")in Symbargs or instruction.getAttribute("opcode")in VarSymbargs or instruction.getAttribute("opcode")in VarSymSymargs or instruction.getAttribute("opcode")in LabelSymSymargs):
                if(typer[0].getAttribute("type")=="int" or typer[0].getAttribute("type")=="string" or typer[0].getAttribute("type")=="bool" or typer[0].getAttribute("type")=="nil" or typer[0].getAttribute("type")=="var"):
                    self.AddArgs(instruction,count,string,typer)
                else:
                    exit(32)
            else:
                exit(32)

    def MoreLabelArgs(self,instruction,count,string):
        if not count in XML_file:
            XML_file[count] = {}
        typer = instruction.getElementsByTagName(string)
        if len(typer) == 0:
            exit(32)
        else:
            if(instruction.getAttribute("opcode")in Labelargs or instruction.getAttribute("opcode")in LabelSymSymargs):
                if(typer[0].getAttribute("type")=="label"):
                    self.AddArgs(instruction,count,string,typer)
                else:
                    exit(32)
            else:
                exit(32)

    def Load_XML(self):
        try:
            DOMTree = xml.dom.minidom.parse(self.file)
        except:
            exit(31)
        collection = DOMTree.documentElement
    
        if not collection.hasAttribute("language"):
            exit(32)

        if not collection.getAttribute("language").upper() == "IPPCODE19":
            exit(32)

        self.LoadArgs(collection)

        return True

    def CheckOrder(self):
        count= len(XML_file.keys())
        i=1
        while(i<=count):
            if not str(i) in XML_file:
                exit(32)
            i+=1

        return True
        
    def GetFile(self):
        return XML_file

class LexSynAnalyzer():
    def __init__(self):
        pass

    def CheckInstruction(self):
        i=int(1)
        count= len(XML_file.keys())
        while(i<=count):
            k=str(i)
            if not str(k) in XML_file:
                return False
            if not(XML_file[k]["instruction"] in Noneargs or XML_file[k]["instruction"] in Labelargs or XML_file[k]["instruction"] in Varsargs or XML_file[k]["instruction"] in Symbargs or XML_file[k]["instruction"] in VarSymbargs or XML_file[k]["instruction"] in VarTypeargs or XML_file[k]["instruction"] in VarSymSymargs or XML_file[k]["instruction"] in LabelSymSymargs):
                return False
            elif(XML_file[k]["instruction"] in Noneargs):
                pass
            elif(XML_file[k]["instruction"] in Varsargs or XML_file[k]["instruction"] in Symbargs or XML_file[k]["instruction"] in Labelargs):
                self.CheckTypes("arg1",k)
            elif(XML_file[k]["instruction"] in VarSymbargs or XML_file[k]["instruction"] in VarTypeargs):
                self.CheckTypes("arg1",k)
                self.CheckTypes("arg2",k)
            elif(XML_file[k]["instruction"]) in VarSymSymargs or XML_file[k]["instruction"] in LabelSymSymargs:
                self.CheckTypes("arg1",k)
                self.CheckTypes("arg2",k)                    
                self.CheckTypes("arg3",k)
            i+=1
        return True


    def CheckValues(self,string,typer,i):
        if(typer=="label"):
            if re.match(r'^[a-z|A-Z|_\-$&%!\?*][0-9|a-z|A-Z|_\-$&%*!\?]*',XML_file[i][string]["value"]):
                return True
            else:
                exit(32)
                
        elif(typer=="int"):
            if re.match(r'^(\+|-)?([0-9]+)$',XML_file[i][string]["value"]):
                return True
            else:
                exit(32)
        elif(typer=="var"):
            if re.match(r'^(G|T|L)F@([a-z|A-Z|_\-$&%!\?*][0-9|a-z|A-Z|_\-$&%*!\?]*)',XML_file[i][string]["value"]):
                return True
            else:
                exit(32)
        elif(typer=="bool"):
            if re.match(r'(true)|(false)',XML_file[i][string]["value"]):
                return True
            else:
                exit(32)
        if(typer=="string"):
            if re.match("(\\\\\d{3,}|[^\\\\\s|#])*",XML_file[i][string]["value"]):
                return True
            else:
                exit(32)
        else:
            exit(32)

    def CheckTypes(self,string,i):
        string=str(string)
        if XML_file[i][string]["type"] == "label":
            self.CheckValues(string,"label",i)
            return True

        elif XML_file[i][string]["type"] == "int":
            self.CheckValues(string,"int",i)
            return True

        elif XML_file[i][string]["type"] == "string":

            self.CheckValues(string,"string",i)
            return True

        elif XML_file[i][string]["type"] == "bool":
            self.CheckValues(string,"bool",i)
            return True

        elif XML_file[i][string]["type"] == "var":
            self.CheckValues(string,"var",i)
            return True
        
        else:
            exit(32)

    def CheckAll(self):
        if self.CheckInstruction() == False:
            exit(32)
        return True



xmlLoader = LoadXML(source["source"])

result = xmlLoader.Load_XML()        
result = xmlLoader.CheckOrder()
lsa= LexSynAnalyzer()