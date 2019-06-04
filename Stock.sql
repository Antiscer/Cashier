select z.NumCode,z.[Name],sum(q) Quantity
from (
SELECT bill.NumCode, bill.[name], reg.SP411 q 
from bill
inner join yellow_direct.DB_1CTRWR.dbo.sc84 nom 
  on nom.CODE = bill.NumCode
inner join yellow_direct.DB_1CTRWR.dbo.RG405 reg
       on nom.[id]=reg.SP408
WHERE reg.PERIOD=@FIRSTDAYINPERIOD
AND reg.SP4062='     0   ' 
and reg.SP418='     FTRG'
union all 
select bill.NumCode, bill.[name],case ret.flag when 1 then -ret.Quantity when -1 then ret.Quantity end q 
from bill
inner join yellow_direct.DB_CASH.dbo.retail ret on bill.NumCode = ret.NumCode
WHERE (ret.flag=1 or ret.flag=-1) 
and ret.SCash in
   (select kkm.descr from yellow_direct.DB_1CTRWR.dbo.sc8071 kkm where kkm.SP8077='     FTRG') --'     KTRG'  
union all 
select bill.NumCode, bill.[name],-bill.Quantity q
from bill
)z
group by z.NumCode,z.[Name]
having sum(q) < 0