# PAL

## 主要學習目標
語法解析

## 說明
PAL課程要求實作OurC(類似C語言)之interpreter，必須能針對該語言進行錯誤偵測、語法判讀、命令執行以及函數呼叫等功能。  
為了實現錯誤偵測及語法判讀，我實作了Dynamic_Recursive_Descent_Parser，它的特殊之處在於，它能將"文法規則"以字串的形式作為參數傳入，若目標語言改變，
只要更改文法規則，即可實現對該語言的語法檢查能力，定義方式可參考程式514~531行。 
由於上述特性，雖然project1與project2~4的文法有所不同，但皆使用此Parser即可。

## 時間
大三下學期，修習PAL課程期間

## 使用語言
C++
