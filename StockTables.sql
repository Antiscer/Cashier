USE DB_CASH

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreStockType') 
BEGIN 
	CREATE TABLE [dbo].[PreStockType](
		[Id] [int] NOT NULL,
		[Description] [nvarchar](100) NULL,
		CONSTRAINT PK_StockType PRIMARY KEY (Id)
)
	INSERT INTO [dbo].[PreStockType] ([Id] ,[Description])
     VALUES (500,'По отдельной позиции')
	INSERT INTO [dbo].[PreStockType] ([Id] ,[Description])
     VALUES (501,'По группе товаров')
	INSERT INTO [dbo].[PreStockType] ([Id] ,[Description])
     VALUES (502,'Множественные условия')
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PrePresentType') 
BEGIN 
	CREATE TABLE [dbo].[PrePresentType](
		[Id] [int] NOT NULL,
		[Description] [nvarchar](100) NULL,
		CONSTRAINT PK_PresentType PRIMARY KEY (Id)
)
	INSERT INTO [dbo].[PrePresentType] ([Id],[Description])
     VALUES (300,'Карта')
	INSERT INTO [dbo].[PrePresentType] ([Id],[Description])
     VALUES (301,'Товар')
END


IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreStock') 
BEGIN 
	CREATE TABLE [dbo].[PreStock](
		[IDStock] [binary](16) NOT NULL PRIMARY KEY,
		[Active] [bit],
		[StartStock] [datetime] NOT NULL,
		[StopStock] [datetime] NOT NULL,
		[ImpStart] [datetime] NULL,
		[ImpStop] [datetime] NULL,
		[StockType] [int] NOT NULL,
		[Vendor] [binary](16) NOT NULL,
		[Description] [nvarchar](512) NULL,
		[WavFileName] [nvarchar](50) NULL,
		[MaxPresentCount] [int] NOT NULL DEFAULT -1,
		[CountUP] [int]	NOT NULL DEFAULT -1,
		CONSTRAINT FK_StockType FOREIGN KEY (StockType) REFERENCES PreStockType(id)
) 
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreSklad') 
BEGIN 
	CREATE TABLE [dbo].[PreSklad](
		[IDStock] [binary](16) NOT NULL,
		[Sklad] [binary](16) NOT NULL,
		CONSTRAINT PK_SkladTable PRIMARY KEY ([IDStock],[Sklad]),
		CONSTRAINT FK_IDStock FOREIGN KEY ([IDStock]) REFERENCES PreStock([IDStock])
)
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreProduct') 
BEGIN 
	CREATE TABLE [dbo].[PreProduct](
		[IDnom] [binary](16) NOT NULL,
		[IDStock] [binary](16) NOT NULL,
		[MinQuantity] [numeric](15,3) NOT NULL DEFAULT 1,
		[MinSum] [numeric](15,2) NOT NULL DEFAULT 0,
		CONSTRAINT PK_Product PRIMARY KEY (IDNom,IDStock),
		CONSTRAINT FK_Product_IDStock FOREIGN KEY ([IDStock]) REFERENCES PreStock([IDStock])
)
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreConditions') 
BEGIN 
	CREATE TABLE [dbo].[PreConditions](
		[IDStock] [binary](16) NOT NULL,
		[Priority] [int] NOT NULL,
		[Condition] [nvarchar](1024) NOT NULL,
		CONSTRAINT PK_Condition PRIMARY KEY ([IDStock],[Priority]),
		CONSTRAINT FK_Condition_IDStock FOREIGN KEY ([IDStock]) REFERENCES PreStock([IDStock])
) 
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PrePresent') 
BEGIN 
	CREATE TABLE [dbo].[PrePresent](
		[IDStock] [binary](16) NOT NULL,
		[Priority] [int] NOT NULL,
		[IDNom] [binary](16) NOT NULL,
		[PresentType] [int] NOT NULL FOREIGN KEY REFERENCES PrePresentType(id),
		[quantity] [numeric](15,3) NOT NULL DEFAULT 1,
		[SpecPrice] [numeric](15,2) NULL,
		[MinSumma] [numeric](15, 2) NOT NULL DEFAULT 0,
		[MinCount] [numeric](15, 3) NOT NULL DEFAULT 0,
		CONSTRAINT PK_Present PRIMARY KEY (IDStock,[Priority],IDNom),
		CONSTRAINT FK_Present_IDStock FOREIGN KEY ([IDStock]) REFERENCES PreStock([IDStock]),
		CONSTRAINT FK_Present_Condition FOREIGN KEY ([IDStock],[Priority]) REFERENCES PreConditions([IDStock],[Priority])
)
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PreGivePresents') 
BEGIN 

CREATE TABLE [dbo].[PreGivePresents](
       [IDStock] [binary](16) NOT NULL,
	   [BillDateTime] datetime NOT NULL,
	   [SCash] char(8) NOT NULL,
       [ScanCode] [nvarchar](12) NOT NULL,
       [BillNumber] [nchar](18) NOT NULL,
       [Sklad] [binary](16) NOT NULL,
       [IDnom] [binary](16) NOT NULL,
       [Quantity] [numeric](15,3) NOT NULL DEFAULT 1,
       [Amount] [numeric](15, 2) NOT NULL
	   CONSTRAINT FK_GivePresent_Stock FOREIGN KEY ([IDStock]) REFERENCES PreStock([IDStock])
) 
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'PotentialPresent') 
BEGIN 

CREATE TABLE [dbo].[PotentialPresent](
	[IDStock] [binary](16) NOT NULL,
	[IDnom] [binary](16) NOT NULL,
	[Quantity] [numeric](13, 3) NOT NULL DEFAULT ((1)),
	[Description] [nvarchar](1024) NULL,
	[WavFileName] [nvarchar](512) NULL,
	[Remain] [int] NULL
)
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'RetPay') 
BEGIN 
CREATE TABLE [dbo].[RetPay](
	[Id] [uniqueidentifier] NOT NULL,
	[BillNumber] [char](18) NOT NULL,
	[PayNum] [int] NOT NULL,
	[PayType] [char](2) NOT NULL,
	[Amount] [numeric](15, 2) NOT NULL,
	[RoundPart] [numeric](2, 2) NULL,
	[Description] [nvarchar](100) NULL,
PRIMARY KEY CLUSTERED 
(
	[Id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'RetCardData') 
BEGIN 
CREATE TABLE [dbo].[RetCardData](
	[RetPayID] [uniqueidentifier] NOT NULL,
	[CardID] [char](25) NULL,
	[CType] [int] NULL,
	[CardName] [nvarchar](33) NULL,
	[SberOwnCard] [int] NULL,
	[RRN] [char](12) NOT NULL,
	[Check] [nvarchar](1000) NOT NULL
) ON [PRIMARY]


ALTER TABLE [dbo].[RetCardData]  WITH CHECK ADD  CONSTRAINT [FK_RetPay] FOREIGN KEY([RetPayID])
REFERENCES [dbo].[RetPay] ([Id])


ALTER TABLE [dbo].[RetCardData] CHECK CONSTRAINT [FK_RetPay]                           
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'Sklad') 
BEGIN 

CREATE TABLE [dbo].[Sklad](
	[id] [smallint] NOT NULL,
	[Sklad] [binary](16) NOT NULL,
	[Name] [nvarchar](25) NULL
) ON [PRIMARY]
SET ANSI_PADDING OFF
INSERT [dbo].[Sklad] ([id], [Sklad], [Name]) VALUES (1, 0x42312020314A20202020204B54524700, N'КОЛОРЛОН')
INSERT [dbo].[Sklad] ([id], [Sklad], [Name]) VALUES (2, 0x42312020314A20202020204654524700, N'ВТД')
INSERT [dbo].[Sklad] ([id], [Sklad], [Name]) VALUES (3, 0x42312020314A20202020205420202000, N'220V')
INSERT [dbo].[Sklad] ([id], [Sklad], [Name]) VALUES (4, 0x42312020314A20202020205A20202000, N'Бердск')
INSERT [dbo].[Sklad] ([id], [Sklad], [Name]) VALUES (5, 0x89B8003048FD651511E06593B00BA2A3, N'БХ')
END


IF NOT EXISTS (SELECT * FROM sysindexes WHERE name = 'IDNOM_IDX' AND id = OBJECT_ID('price'))
BEGIN
	CREATE INDEX IDNOM_IDX ON price (IDNom)
END

-- таблицы доставки
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'DeliveryType')
BEGIN
   CREATE TABLE [dbo].[DeliveryType](
	[Id] int NOT NULL PRIMARY KEY,
	[Name] nvarchar(30)
)

   INSERT INTO DeliveryType (Id, Name) VALUES
	(1, N'Отправка'),
	(2, N'Самовывоз')
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'Delivery')
BEGIN
	CREATE TABLE [dbo].[Delivery](
	[DocID] int IDENTITY NOT NULL,
	[Scancode] char(12) UNIQUE NOT NULL,
	[BillNumber] char(18) NOT NULL,
	[Date] datetime NOT NULL,
	[DeliveryDate] datetime NULL,
	[Type] int NOT NULL
	CONSTRAINT PK_RetDelivery PRIMARY KEY (DocID),
	CONSTRAINT FK_Delivery_Type FOREIGN KEY ([Type]) REFERENCES DeliveryType (Id)
)
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'DeliveryItems')
BEGIN
   	CREATE TABLE [dbo].[DeliveryItems](
	[DocID] int NOT NULL,
	[IDNom] binary(16) NULL,
	[Quantity] [numeric](13,3) NOT NULL,
	[Price] [numeric](14,2)  NOT NULL
	CONSTRAINT FK_Items_Delivery FOREIGN KEY (DocID) REFERENCES Delivery (DocID)
)
END

IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'DeliveryStatusName')
BEGIN
	CREATE TABLE [dbo].[DeliveryStatusName](
	[Id] int NOT NULL PRIMARY KEY,
	[Name] nvarchar(30)
	)

	INSERT INTO DeliveryStatusName (Id, Name) VALUES
	(1, N'Принято'),
	(2, N'Отправлено'),
	(3, N'Частично отправлено')
END
IF NOT EXISTS (
	SELECT table_name FROM information_schema.tables WHERE table_name = 'DeliveryStatus')
BEGIN
	CREATE TABLE [dbo].[DeliveryStatus](
	[DocID] int NOT NULL,
	[Date] [datetime] NOT NULL,
	[Status] [int] NOT NULL,
	[SCash] [char](8) NOT NULL,
	[Operator] [char](15) NOT NULL
	CONSTRAINT FK_DeliveryStatus_Delivery FOREIGN KEY (DocID) REFERENCES Delivery (DocID),
	CONSTRAINT FK_StatusName FOREIGN KEY ([Status]) REFERENCES DeliveryStatusName (Id)
	)
END

