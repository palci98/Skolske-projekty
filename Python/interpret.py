from XML_loader import *
import sys

LABELDICT={}
if source["input"] == None:
    inputfile=None
else:
    try:
        inputfile = open(source["input"])
    except:
        exit(11)

def MoveTo(argument,OpType,operation):
    value={"type": OpType,"value": operation}
    if argument["value"][:3] == "GF@":
        GF.move(argument["value"][3:],value)
    elif argument["value"][:3] == "LF@":
        LF.move(argument["value"][3:],value)
    elif argument["value"][:3] == "TF@":
        TF.move(argument["value"][3:],value)
    else:
        exit(55)
def GetArg(index,argument):
    if not str(index) in XML_file:
        exit(32)

    if argument in XML_file[str(index)]:
        return XML_file[str(index)][argument]
    return None

def GetValue(var):
    if var[:3] == "GF@":
        return GF.get(var[3:])
    if var[:3] == "LF@":
        return LF.get(var[3:])
    if var[:3] == "TF@":
        return TF.get(var[3:])


def Add(argument2,argument3):
    return int(argument2)+int(argument3)

def And(argument2,argument3):
    if argument2 == argument3 == "true" :
        return "true"
    else:
        return "false"

def SetCH(frame,name,argument1,argument2,argument3):
    argument3=argument3["value"][0]
    if name== None:
        exit(53)
    if frame=="TF":
        if TF.TemporaryFrame==None:
            exit(53)
        if name in TF.TemporaryFrame:
            helpstring=(TF.TemporaryFrame[name]["value"])
            string=list(helpstring)
            string[int(argument2["value"])]=argument3
            helpstring= ''.join(string)
            MoveTo(argument1,"string",helpstring)
        else:
            exit(52)
    if frame=="LF":
        if name in LF.LocalFrame:
            helpstring=(LF.LocalFrame[name]["value"])
            string=list(helpstring)
            string[int(argument2["value"])]=argument3
            helpstring= ''.join(string)
            MoveTo(argument1,"string",helpstring)
        else:
            exit(52) 
    if frame=="GF":
        if name in GF.GlobalFrame:
            helpstring=(GF.GlobalFrame[name]["value"])
            string=list(helpstring)
            string[int(argument2["value"])]=argument3
            helpstring= ''.join(string)
            MoveTo(argument1,"string",helpstring)
        else:
            exit(52)

def Or(argument2,argument3):
    if argument2== "true":
        return "true"
    elif argument3 == "true":
        return "true"
    else:
        return "false"

def Not(argument2):
    if(argument2=="true"):
        return "false"
    if(argument2=="false"):
        return "true"

def Sub(argument2,argument3):
    return int(argument2)-int(argument3)

def Mul(argument2,argument3):
    return int(argument2)*int(argument3)

def Div(argument2,argument3):
    if int(argument3)==int(0):
        exit(57)
    else:
        return int(argument2)//int(argument3)

class CallStack:
    def __init__(self):
        self.items= []

    def pushs(self,item):
        self.items.append(item)
    
    def isEmpty(self):
        if len(self.items)==0:
            exit(56)
        else:
            return

    def pops(self):
        self.isEmpty()
        return self.items.pop()

class DataStack:
    def __init__(self):
        self.items= []

    def pushs(self,item):
        self.items.append(item)
    
    def isEmpty(self):
        if self.items ==[]:
            exit(56)
        else:
            return

    def pops(self):
        self.isEmpty()
        return self.items.pop()

class LocalFrame:

    def __init__(self):
        self.LocalFrame = []

    def __contains__(self,var):
        if self:
            return var in self.LocalFrame[-1]
        else:
            exit(55)

    def __bool__(self):
        return len(self.LocalFrame) != 0

    def pushframe(self,frame):
        self.LocalFrame.append(frame)

    def popframe(self):
        if self:
            return self.LocalFrame.pop()
        exit(55)

    def define(self,var):
        if var in self:
            exit(52)
        self.LocalFrame[-1][var] = None                

    def move(self,var,value):
        if not var in self:
            exit(54)
        self.LocalFrame[-1][var] = value

    def get(self,var):
        if var in self:    
            return self.LocalFrame[-1][var]
        else:
            exit(54)

class GlobalFrame:

    def __init__(self):
        self.GlobalFrame={}
    
    def __contains__(self,var):
        return var in self.GlobalFrame

    def define(self,var):
        if var in self:
            exit(52)
        self.GlobalFrame[var] = None

    def move(self,var,value):
        if not var in self:
            exit(54)
        self.GlobalFrame[var] = value

    def get(self,var):
        if var in self:
            return self.GlobalFrame[var]
        exit(54)
    

class TemporaryFrame:

    def __init__(self):
        self.TemporaryFrame=None

    def inicialize(self):
        self.TemporaryFrame={}        

    def pushframe(self,frame):
        self.TemporaryFrame = frame

    def popframe(self):
        if not self:
            exit(55)
        tmp = self.TemporaryFrame
        self.TemporaryFrame=None
        return tmp

    def __contains__(self,var):
        if self.TemporaryFrame == None:
            exit(55)
        return var in self.TemporaryFrame

    def __bool__(self):
        return self.TemporaryFrame != None
           
    def define(self,var):
        if var in self:
            exit(52)
        self.TemporaryFrame[var] = None

    def move(self,var,value):
        if not var in self:
            exit(54)
        self.TemporaryFrame[var] = value

    def get(self,var):
        if var in self:
            return self.TemporaryFrame[var]
        exit(54)
        
STACK = DataStack()
CS = CallStack()
TF = TemporaryFrame()
LF = LocalFrame()
GF = GlobalFrame()

count = len(XML_file.keys())
index = 1

while(index <= count):
    if not str(index) in XML_file:
        exit(32)
    instruction = XML_file[str(index)]["instruction"]
    argument1 = GetArg(index,"arg1")
    
    if instruction == "LABEL":
        if argument1["type"] == "label":
            if argument1["value"] in LABELDICT:
                exit(52)
            else:
                LABELDICT[argument1["value"]]=index
                LABELDICT["name"]=argument1["value"]
    index += 1


count = len(XML_file.keys())
index = 1

while(index <= count):

    instruction = XML_file[str(index)]["instruction"]
    argument1 = GetArg(index,"arg1")
    argument2 = GetArg(index,"arg2")
    argument3 = GetArg(index,"arg3")

    
    if instruction == "DEFVAR":
        if argument1["value"][:3] == "GF@":
            GF.define(argument1["value"][3:])
        elif argument1["value"][:3] == "LF@":
            LF.define(argument1["value"][3:])
        elif argument1["value"][:3] == "TF@":
            TF.define(argument1["value"][3:])
        else:
            exit(32)

    if instruction == "CREATEFRAME":
        TF.inicialize()

    if instruction == "PUSHFRAME":
        LF.pushframe(TF.popframe())
 
    if instruction == "POPFRAME":
        TF.pushframe(LF.popframe())
    
    if instruction == "MOVE":
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2        
        MoveTo(argument1,value["type"],value["value"])
        

    if instruction == "CALL":
        if argument1["type"]=="label":
            if argument1["value"] in LABELDICT:
                CS.pushs(int(index))
                index= int (LABELDICT[argument1["value"]])-1
            else:
                exit(52)
        else:
            exit(53)

    if instruction == "RETURN":
        index=CS.pops()



# ----------------------------------------------------------------------
# --------Aritmeticke, relacne, boolovske a konverzne instrukci---------
# ----------------------------------------------------------------------


    if instruction == "ADD":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == value2["type"] == "int":
            sumarize= Add(value["value"],value2["value"])
            MoveTo(argument1,"int",sumarize)
        else:
            exit(53)


    if instruction == "SUB":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == value2["type"] == "int":
            Subtraction= Sub(value["value"],value2["value"])
            MoveTo(argument1,"int",Subtraction)

        else:
            exit(53)

    if instruction == "MUL":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == value2["type"] == "int":
            Multiplication= Mul(value["value"],value2["value"])
            MoveTo(argument1,"int",Multiplication)

        else:
            exit(53)

    if instruction == "IDIV":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == value2["type"] == "int":
            Divide= Div(value["value"],value2["value"])
            MoveTo(argument1,"int",int(Divide))

        else:
            exit(53)


    if instruction == "LT":
        if argument1 == None:
            exit(54)
            
        if argument2["type"] == "var":
            value2 = GetValue(argument2["value"])
        else:
            value2 = argument2

        if argument3["type"] == "var":
            value3 = GetValue(argument3["value"])
        else:
            value3 = argument3
        
        if value2["type"]==value3["type"] == "int":
            if int(value2["value"]) < int(value3["value"]):
                MoveTo(argument1,"bool","true")
            else:
                MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "bool":
            if value2["value"] == "false":
                if value3["value"]=="true":
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
            if value2["value"] =="true":
                MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "string":
            ordinal1=0
            ordinal2=0
            for idx in range(min(len(value2["value"]), len(value3["value"]))):
                ordinal1 += ord(value2["value"][idx])
                ordinal2 += ord(value3["value"][idx])
                if ordinal1 == ordinal2:
                    continue
                elif ordinal1 < ordinal2:
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
        else:
            exit(53)

    if instruction == "EQ":
        if argument1 == None:
            exit(54)
        
        if argument2["type"] == "var":
            value2 = GetValue(argument2["value"])
        else:
            value2 = argument2

        if argument3["type"] == "var":
            value3 = GetValue(argument3["value"])
        else:
            value3 = argument3

        if value2 == value3 ==None:
            exit(56)
        
        if value2["type"]== "nil":
            if value3["type"] != "nil":
                MoveTo(argument1,"bool","false")
            else:
                MoveTo(argument1,"bool","true")
        elif value3["type"]== "nil":
            if value2["type"] != "nil":
                MoveTo(argument1,"bool","false")
            else:
                MoveTo(argument1,"bool","true")
        elif value2["type"]==value3["type"]=="nil":
            MoveTo(argument1,"bool","true")

        elif value2["type"]==value3["type"] == "int":
            if int(value2["value"])==int(value3["value"]):
                MoveTo(argument1,"bool","true")
            else:
                MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "bool":
            if value2["value"] == "true":
                if value3["value"]=="true":
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
            if value2["value"] =="false":
                if value3["value"]=="false":
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "string":
            ordinal1=0
            ordinal2=0
            for idx in range(min(len(value2["value"]), len(value3["value"]))):
                ordinal1 += ord(value2["value"][idx])
                ordinal2 += ord(value3["value"][idx])
                if ordinal1 == ordinal2:
                    continue
                elif ordinal1 == ordinal2:
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
        else:
            exit(53)

    if instruction == "GT":

        if argument1 == None:
            exit(54)

        if argument2["type"] == "var":
            value2 = GetValue(argument2["value"])
        else:
            value2 = argument2

        if argument3["type"] == "var":
            value3 = GetValue(argument3["value"])
        else:
            value3 = argument3
        
        if value2 == None or value3==None:
            exit(56)
        
        if value2["type"]==value3["type"] == "int":
            if int(value2["value"])>int(value3["value"]):
                MoveTo(argument1,"bool","true")
            else:
                MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "bool":
            if value2["value"] == "true":
                if value3["value"]=="false":
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
            if value2["value"] =="false":
                MoveTo(argument1,"bool","false")

        elif value2["type"]==value3["type"] == "string":
            ordinal1=0
            ordinal2=0
            for idx in range(min(len(value2["value"]), len(value3["value"]))):
                ordinal1 += ord(value2["value"][idx])
                ordinal2 += ord(value3["value"][idx])
                if ordinal1 == ordinal2:
                    continue
                elif ordinal1 > ordinal2:
                    MoveTo(argument1,"bool","true")                        
                else:
                    MoveTo(argument1,"bool","false")
        else:
            exit(53)

    if instruction == "AND":
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3
    
        if value["type"] == value2["type"] == "bool":
            AndResult=And(value["value"], value2["value"])
            MoveTo(argument1,"bool",AndResult)
        else:
            exit(53)

    if instruction == "OR":
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3
    
        if value["type"] == value2["type"] == "bool":
            OrResult=Or(value["value"], value2["value"])
            MoveTo(argument1,"bool",OrResult)
        else:
            exit(53)
    
    if instruction == "NOT":
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2
    
        if value["type"] == "bool":
            NotResult=Not(value["value"])
            MoveTo(argument1,"bool",NotResult)
        else:
            exit(53)
    
    if instruction == "INT2CHAR":
        if argument1 == None:
            exit(54)
        
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2
        if int(value["value"]) <0:
                exit(58)
        if value["type"] == "int":
            MoveTo(argument1,"string",chr(int(value["value"])))
        else:
            exit(53)
    
    if instruction == "STRI2INT":
        if argument1 == None:
            exit(54)
        

        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value["type"] == "string" and value2["type"] == "int":
            if 0 > int(value2["value"]) or int(value2["value"]) >= len(value["value"]):
                exit(58)
            MoveTo(argument1,"int",ord(value["value"][int(value2["value"])]))
        else:
            exit(53)
# ---------------------------------------------------
# ---------PRACA S DATOVYM ZASOBNIKOM----------------
# ---------------------------------------------------

    if instruction == "PUSHS":
        if argument1["type"] == "var":
            value = GetValue(argument1["value"])
        else:
            value = argument1
        if value==None:
            exit(56)
        pushvalue={"type": value["type"],"value": value["value"]}
        STACK.pushs(pushvalue)

    if instruction == "POPS":
        value=STACK.pops()
        MoveTo(argument1,value["type"],value["value"])
# ---------------------------------------------------
# ------------VSTUPNO VYSTUPNE INSTRUKCE-------------
# ---------------------------------------------------

    if instruction == "WRITE":
        if argument1 is None:
            exit(56)
        if argument1["type"] == "var":
            value = GetValue(argument1["value"])
        else:
            value = argument1

        if value == None:
            exit(56)
        
        if value["type"] != "nil":
            print(value["value"],end='')
        
    if instruction == "READ":
        
        if argument1 is None:
            exit(54)

        if argument1["type"] == "var":
            
            value = GetValue(argument1["value"])
            if argument2["type"]=="type":
                if argument2["value"]=="int":
                    if inputfile == None:
                        try:
                            line = input()
                        except:
                            line=""
                        try:
                            line=int(line)
                        except:
                            exit(53)
                        MoveTo(argument1,"int",line)
                        
                    else:
                        line = inputfile.readline()
                        try:
                            line=int(line)
                        except:
                            exit(53)
                        MoveTo(argument1,"int",line)

                elif argument2["value"]=="string":
                    if inputfile == None:
                        try:
                            line = input()
                        except:
                            line=""
                        MoveTo(argument1,"string",line)
                    else:
                        line = inputfile.readline()
                        MoveTo(argument1,"string",line)

                elif argument2["value"]=="bool":
                    if inputfile == None:
                        try:
                            line = input()
                        except:
                            line=""
                        if line=="true":
                            MoveTo(argument1,"bool","true")
                        else:
                            MoveTo(argument1,"bool","false")
                    else:
                        line = inputfile.readline()
                        if line=="true":
                            MoveTo(argument1,"bool","true")
                        else:
                            MoveTo(argument1,"bool","false")
                else:
                    exit(53)

            else:
                exit(53)
        else:
            exit(53)


# ---------------------------------------------------
# --------------PRACA S RETAZCAMI--------------------
# ---------------------------------------------------

    if instruction == "CONCAT":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == value2["type"] == "string":
            MoveTo(argument1,"string",str(value["value"])+str(value2["value"]))
        else:
            exit(53)

    if instruction == "STRLEN":
        if argument1 == None:
            exit(54)

        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if value==None:
            exit(56)
        
        if value["type"] == "string":
            MoveTo(argument1,"string",len(value["value"]))
        else:
            exit(53)

    
    if instruction == "GETCHAR":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3

        if value==None or value2==None:
            exit(56)

        if value["type"] == "string" and value2["type"]=="int":
            if 0 > int(value2["value"]) or int(value2["value"]) >= len(value["value"]):
                exit(58)
            MoveTo(argument1,"string",value["value"][int(value2["value"])])
        else:
            exit(53)

    if instruction == "SETCHAR":
        if argument1 == None:
            exit(54)
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2

        if argument3["type"] == "var":
            value2 = GetValue(argument3["value"])
        else:
            value2 = argument3
        arg1string= GetValue(argument1["value"])

        if arg1string==None or value==None or value2==None:
            exit(56)
        if  arg1string["type"] == "string" and value["type"] == "int" and value2["type"]=="string":
            if 0 > int(value["value"]) or int(value["value"]) >= len(value2["value"]):
                exit(58)
            var=argument1["value"]
            frame, name = var.split('@')
            SetCH(frame,name,argument1,value,value2)
        else:
            exit(53)


# ---------------------------------------------------
# --------------Ladiacie instrukcie------------------
# ---------------------------------------------------

    if instruction == "DPRINT":
        print(argument1["value"],file=sys.stderr)

    if instruction == "BREAK":
        print("TemporaryFrame:",file=sys.stderr)
        print(TF.TemporaryFrame,file=sys.stderr)
        print("LocalFrame:",file=sys.stderr)
        print(LF.LocalFrame,file=sys.stderr)
        print("GlobalFrame:",file=sys.stderr)
        print(GF.GlobalFrame,file=sys.stderr)
        print("Stack:",file=sys.stderr)
        print(STACK.items,file=sys.stderr)

# ---------------------------------------------------
# -----------------Prace s typy----------------------
# ---------------------------------------------------

    if instruction == "TYPE":
        if argument2["type"] == "var":
            value = GetValue(argument2["value"])
        else:
            value = argument2
        
        if argument1 == None:
            exit(53)

        if value == None:
            MoveTo(argument1,"string","")
        else:
            MoveTo(argument1,"string",value["type"])   

# ---------------------------------------------------
# --------INSTRUKCE PRO RIZENI TOKU PROGRAMU---------
# ---------------------------------------------------

    if instruction == "JUMP":
        
        if argument1["value"] not in LABELDICT:
            exit(52)
        if argument1["type"] == "label":
            index= int(LABELDICT[argument1["value"]]) -1
        else:
            exit(53)

    if instruction == "JUMPIFEQ":

        if argument1["type"] == "label":
            
            if argument2["type"] == "var":
                value2 = GetValue(argument2["value"])
            else:
                value2 = argument2

            if argument3["type"] == "var":
                value3 = GetValue(argument3["value"])
            else:
                value3 = argument3
            
            if argument1["value"] not in LABELDICT:
                exit(52)

            else:
                if value2["type"]==value3["type"] == "nil":
                        index= LABELDICT[argument1["value"]]

                elif value2["type"]==value3["type"] == "int":
                    if value2["value"]==value3["value"]:
                        index= LABELDICT[argument1["value"]]

        
                elif value2["type"]==value3["type"] == "bool":
                    if value2["value"] == "true":
                        if value3["value"]=="true":
                            index= LABELDICT[argument1["value"]]

                    if value2["value"] =="false":
                        if value3["value"]=="false":
                            index= LABELDICT[argument1["value"]]

                elif value2["type"]==value3["type"] == "string":
                    ordinal1=0
                    ordinal2=0
                    for idx in range(min(len(value2["value"]), len(value3["value"]))):
                        ordinal1 += ord(value2["value"][idx])
                        ordinal2 += ord(value3["value"][idx])
                        if ordinal1 == ordinal2:
                            continue
                        elif ordinal1 == ordinal2:
                            index= LABELDICT[argument1["value"]]
                else:
                    exit(53)
        else:
            exit(53)



    if instruction == "JUMPIFNEQ":

        if argument1["type"] == "label":
            
            if argument2["type"] == "var":
                value2 = GetValue(argument2["value"])
            else:
                value2 = argument2

            if argument3["type"] == "var":
                value3 = GetValue(argument3["value"])
            else:
                value3 = argument3
            
            if argument1["value"] not in LABELDICT:
                exit(52)

            else:
                if value2["type"]==value3["type"] == "nil":
                        index= LABELDICT[argument1["value"]]


                elif value2["type"]==value3["type"] == "int":
                    if value2["value"]!=value3["value"]:
                        index= LABELDICT[argument1["value"]]
        
                elif value2["type"]==value3["type"] == "bool":
                    if value2["value"] == "true":
                        if value3["value"]!="true":
                            index= LABELDICT[argument1["value"]]
                    if value2["value"] =="false":
                        if value3["value"]!="false":
                            index= LABELDICT[argument1["value"]]


                elif value2["type"]==value3["type"] == "string":
                    ordinal1=0
                    ordinal2=0
                    for idx in range(min(len(value2["value"]), len(value3["value"]))):
                        ordinal1 += ord(value2["value"][idx])
                        ordinal2 += ord(value3["value"][idx])
                        if ordinal1 == ordinal2:
                            continue
                        elif ordinal1 != ordinal2:
                            index= LABELDICT[argument1["value"]]
                else:
                    exit(53)
        else:
            exit(53)


    if instruction == "EXIT":
        if argument1["type"] == "var":
            value = GetValue(argument1["value"])
        else:
            value = argument1

        if value==None:
            exit(56)
        if value["type"]=="int":
            if 0<= int(value["value"]) <=49:
                exit(int(value["value"]))
            else:
                exit(57)
        else:
            exit(53)
    index += 1