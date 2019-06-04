Set objConn = CreateObject("ADODB.Connection")
'Определяем параметры подключения к базе данных
ConnectString = "Provider=SQLOLEDB;Data Source=colkas5\sqlexpress;Initial Catalog=DB_CASH;Integrated Security=SSPI;"
objConn.ConnectionString = ConnectString
'Подключаемся к базе данных
objConn.Open
'Перебираем коллекцию ошибок
Set objRecordset = objConn.Execute("EXECUTE sp_executesql N'WITH getQA (Quantity) AS (select 1) select * from getQA'")
While Not objRecordset.EOF
    strRes = vbNullString
    For i=0 To objRecordset.Fields.Count-1
        strRes = strRes & CStr(objRecordset.Fields(i).Value) & vbTab
    Next
    WScript.Echo Trim(strRes)
    objRecordset.MoveNext
Wend
For Each E in objConn.Errors
    WScript.Echo "Error.Description: " & E.Description
    WScript.Echo "Error.Number: " & E.Number
    WScript.Echo "Error.Source: " & E.Source
    WScript.Echo "Error.SQLState: " & E.SQLState
    WScript.Echo "Error.NativeError: " & E.NativeError
    WScript.Echo
Next
'Закрываем соединение
objConn.Close
Set objConn = Nothing