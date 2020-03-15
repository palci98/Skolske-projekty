<?php

$instructions=array("MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL",
                        "RETURN", "PUSHS", "POPS", "ADD", "SUB", "MUL", "IDIV", "LT", "GT",
                        "EQ", "AND", "OR", "NOT", "INT2CHAR", "STRI2INT", "READ", "WRITE",
                        "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "TYPE", "LABEL", "JUMP",
                        "JUMPIFEQ", "JUMPIFNEQ", "EXIT", "DPRINT", "BREAK");

const header = 10;
const variable = 11;
const constant_int = 22;
const constant_bool = 23;
const constant_nil = 24;
const constant_string = 25;
const instruction = 3;
const label = 17;
const type = 15;

const tokenEOF = 5;
const tokenEOL = 7;

// kontrola argumentov 
function CheckArguments($argc,$argv)
{
  if($argc==1)
  {
    return;
  }

  else if ($argc == 2 && $argv[1]=="--help")
  {
    echo "Program nacita subor zo standardneho vstupu <subor\n";
    exit (0);
  }
  else
  {
    echo "pre napovedu zadajte ako argument --help\n";
    exit (10);
  }
}

CheckArguments($argc,$argv);

/*
 *  Funkcia vracia vzdy jeden riadok 
 *  Implementovana ako konecny automat
 */  

function GetToken()
{
    // Specialne znaky, ktore sa mozu nachadzat na zaciatku premennej
    global $instructions;
    $specialchars=array("_","-","$","&","%","*","!","?");
    $returnedtoken = array();
    $state = "default";
    $ch = "";
    $ch2 = "";
    $word = "";
    $count = 0;

    while(true)
    {
        if ($ch2 != "")
        {
          $ch = $ch2;
          $ch2 = "";
        }
        else
        {
          $ch=fgetc(STDIN); // nacitanie postupne po znakoch
        }

        switch($state)
        {
            default:
            case "default":


                if(ord($ch)==0)
                {
                    array_push($returnedtoken,tokenEOF);
                    return $returnedtoken;
                }
                if($ch == ".")
                {
                    $word = $word.$ch;
                    $state = "header";
                    continue 2;
                }
                if($ch == "#")
                {
                    $state="comment_line";
                    continue 2;
                }

                if($ch == PHP_EOL) 
                {
                    array_push($returnedtoken,tokenEOL);
                    return($returnedtoken);
                }

                if(ctype_alpha($ch))
                {
                    $word = $word . $ch;
                    $state="possible vars or types";
                    continue 2;
                }

                if(ctype_space($ch)||$ch=="\t")
                {
                    $state = "default";
                    continue 2;
                }
                else
                {
                    exit(23);
                }
                break;

            case "possible vars or types":
                    if($word == "GF@")
                    {
                      for($i=0;$i<count($specialchars);$i++)
                      {
                        if($ch==$specialchars[$i]||ctype_alpha($ch))
                        {
                          $word=$word.$ch;
                          $state="GF";
                          continue 3;
                        }
                      }
                      exit(23);
                    }

                    if($word == "LF@")
                    {
                      for($i=0;$i<count($specialchars);$i++)
                      {
                        if($ch==$specialchars[$i]||ctype_alpha($ch))
                        {
                          $word=$word.$ch;
                          $state="LF";
                          continue 3;
                        }
                      }
                      exit(23);
                    }

                    if($word == "TF@")
                    {
                      for($i=0;$i<count($specialchars);$i++)
                      {
                        if($ch==$specialchars[$i]||ctype_alpha($ch))
                        {
                          $word=$word.$ch;
                          $state="TF";
                          continue 3;
                        }
                      }
                      exit(23);
                    }

                    if($ch== "@")
                    {
                        if($word=="bool")
                        {
                            $word="";
                            $state="bool";
                            continue 2;
                        }
                        if($word=="int")
                        {
                            $word="";
                            $state="int";
                            continue 2;
                        }
                        if($word=="string")
                        {
                            $word="";
                            $state="string";
                            continue 2;
                        }
                        if($word=="nil")
                        {
                            $word="";
                            $state="nil";
                            continue 2;
                        }
                    }

                    else if ($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                    {
                      // Kontrola ci sa jedna o instrukciu
                      for($i=0;$i<count($instructions);$i++)
                      {
                          if(strtoupper($word) == strtoupper($instructions[$i]))
                          {
                            array_push($returnedtoken,array(instruction,$word));
                            $state = "default";
                            $word = "";
                            $ch2 = $ch;
                            continue 3;
                        }
                      }

                      // Kontrola ci sa jedna o label
                      for ($i=0; $i < strlen($word); $i++)
                      {
                        if (ctype_digit($word[$i]))
                        {
                          if ($i == 0)
                          {
                            exit(23);
                          }
                          continue;
                        }
                        if (ctype_alpha($word[$i]))
                        {
                          continue;
                        }
                        if ($word[$i] == "_" || $word[$i] == "-" || $word[$i] == "$" || $word[$i] == "&" || $word[$i] == "%" || $word[$i] == "*" || $word[$i] == "!" || $word[$i] == "?")
                        {
                          continue;
                        }
                        exit(23);
                      }

                      array_push($returnedtoken,array(label,$word));
                      $state = "default";
                      $word = "";
                      $ch2 = $ch;
                      continue 2;
                    }

                    $word=$word.$ch;


                    break;

            case "GF":
                    if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                    {
                        array_push($returnedtoken,array(variable,$word));
                        $word="";
                        $ch2 = $ch;
                        $state="default";
                        continue 2;
                    }
                    $word=$word.$ch;
            break;

            case "LF":
                    if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                    {
                        array_push($returnedtoken,array(variable,$word));
                        $word="";
                        $ch2 = $ch;
                        $state="default";
                        continue 2;
                    }
                    $word=$word.$ch;
            break;

            case "TF":
                    if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                    {
                        array_push($returnedtoken,array(variable,$word));
                        $word="";
                        $ch2 = $ch;
                        $state="default";
                        continue 2;
                    }
                    $word=$word.$ch;

            break;

            case "nil":
                    $word=$word.$ch;
                    if($word=="nil")
                    {
                        $ch=fgetc(STDIN);
                        if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                        {
                          array_push($returnedtoken,array(constant_nil,$word));
                          $word = "";
                          $ch2 = $ch;
                          $state = "default";
                          continue 2;
                        }
                        else
                        {
                            exit(23);
                        }
                    }
                    if (strlen($word) >= 3 || ord($ch) == 0)
                    {
                      exit(23);
                    }
            break;

            case "bool":
                    $word=$word.$ch;
                    if(strlen($word) == 4)
                    {
                        if($word=="true")
                        {
                            $ch=fgetc(STDIN);
                            if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                            {
                              array_push($returnedtoken,array(constant_bool,$word));
                              $word = "";
                              $ch2 = $ch;
                              $state = "default";
                              continue 2;
                            }
                            else
                            {
                                exit(23);
                            }
                        }
                    }
                    if(strlen($word) == 5)
                    {
                        if($word=="false")
                        {
                            $ch=fgetc(STDIN);
                            if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                            {
                              array_push($returnedtoken,array(constant_bool,$word));
                              $word = "";
                              $ch2 = $ch;
                              $state = "default";
                              continue 2;
                            }
                            else
                            {
                                exit(23);
                            }
                        }
                    }
                    if (ord($ch) == 0)
                    {
                      exit(23);
                    }
                    break;

            case "int":
                    $word = $ch;
                    if ($ch == "+")
                    {
                      $state = "numberp";
                    }
                    else if ($ch == "-")
                    {
                      $state = "numberp";
                    }
                    else if (ctype_digit($ch))
                    {
                      $state = "numberp";
                    }
                    else
                    {
                      exit(23);
                    }
                    break;
            case "numberp":
                    if($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                    {
                        $ch2 = $ch;
                        array_push($returnedtoken,array(constant_int,$word));
                        $word = "";
                        $state = "default";
                        continue 2;
                    }
                    if(ctype_digit($ch))
                    {
                        $word=$word.$ch;
                    }
                    else
                    {
                        exit(23);
                    }
                    break;


            case "string":
                if ($ch == "\n" || $ch == "\r" || $ch == "\t" || ord($ch) == 0 || ctype_space($ch) || $ch == "#")
                {
                  array_push($returnedtoken,array(constant_string,$word));
                  $word="";
                  $state ="default";
                  $ch2 = $ch;
                  continue 2;
                }
                if ($ch == "\\")
                {
                  $specialchar = "";
                  for ($i=0; $i < 3; $i++)
                  {
                    $ch = fgetc(STDIN);

                    if (!ctype_digit($ch))
                    {
                      exit(23);
                    }

                    $specialchar = $specialchar.$ch;
                  }

                  if ($specialchar >= 0 && $specialchar <= 999)
                  {
                    if ($specialchar < 032)
                    {
                      if (!($specialchar == 010 || $specialchar == 012 || $specialchar == 032))
                      {
                        exit(23);
                      }
                    }
                  }
                  $word = $word."\\".$specialchar;
                  continue 2;
                }
                $word = $word.$ch;
              break;
            case "header":
                if(ord($ch)==0)
                {

                  exit(21);
                }

                $word = $word.$ch;

                if(strlen($word) == 10)
                {
                    if(strtoupper($word) == ".IPPCODE19") 
                    {
                        array_push($returnedtoken,array(header));
                        $word="";
                        $state ="default";
                        continue 2;
                    }
                    else
                    {
                      exit(21);

                    }
                  }
                break;

            case "comment_line":
                if($ch==PHP_EOL||ord($ch)==0)
                {
                    array_push($returnedtoken,tokenEOL);
                    return $returnedtoken;
                }
                break;
        }

    }

}