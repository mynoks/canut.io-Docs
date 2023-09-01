# Support Readme
Support readme to use has a reference to develop the support web page.   
Aquí apendràs tot el que fa falta per integrar el fluxe de canut.io a una màquina de ventes. Desde el hardware al software, acabant en un certificat per validar-te com un instalador reconegut per canut.io.

## Table of content:
- [Getting started](#getting-started)
- [Topics](#topics)
    - [NFC](#nfc)
    - [Flux](#flux)
    - [Requirements](#requirements)
- [Hardware](#hardware)
    - [Arduino](#arduino)
    - [PN532](#pn532)
- [Software](#software)
    - [Download arduino IDE](#download-arduino-ide)
    - [Download libraries](#download-libraries)
- [Code](#code)
    - [Introduction](#introduction)
    - [Basics](#basics)
    - [Emulate tag](#emulate-tag)
    - [Validation data](#validation-data)
    - [Custom Response](#custom-response)
- [Canut.io Certificate](#canutio-certificate )
- [FAQ](#faq)
- [Links of interest](#links-of-interest)

## Getting started
Canut.io es una plataforma de micropagaments a través de contactless o NFC.  

## Topics
El fluxe de canut.io utilitza diverses tecnologies i aparells:

### NFC
> Màquina de venta:  
El dispositiu on es vol integrar el sistema de pagament de canut.io. La màquina de venta serà diferent en cada cas, però sempre serà el punt de partida. 

> En cas que no existeixi una màquina de venta física per realitzar el pagament, s'entendrà que estem fabrican un datafon.

> Arduino:  
Responsable de gestionar la comunicació entre la maquina de ventes i l'app smartphone de canut.io. El llenguatge de desenvolupament que utilitza es el C++. Cal afegir llibreries i adaptar la resposta als requeriments de la màquina de ventes.

> PN532:  
El xip que entableix la comunicació NFC amb el smartphone. S'ha de soldar a la board d'Arduino i col·locar adequadament a prop de la superficie de la maquina de ventes per ser accesible desde el smartphone.

> App Canut.io:  
La app de smartphone de canut.io rep les peticions originades en la màquina de venta i les respon desde la nostra API. Segons la informació que rep, confirma o rebutja la venta del producte.


Primer presentarem el elements de hardware necessaris per relacionar-se amb la base de dades. 

## Flux
---
Falta diagrama?
=
El fluxe de Canut.io comença per cobrir la necessitat de realitzar micropagaments en una màquina de ventes. La natura de la màquina de ventes abarca un espectre virtualment il·limitat: desde una màquina de café fins a una màquina multiventa amb diversos productes, pasant per un simple datafon on realitzar el pagament...  
Encara que aquest tutorial deixarà recomenacions, no aprofunditzarà en detall en les màquines de ventes, ja que es resposabilitat de cada empresa decidir quina considera la forma mecànica o programatica d'actuació més adequada a l'hora de proporcionar el producte al client.

> Per exemple: No té la mateixa aplicació una maquinaria que només serveix un sol producte, que una que ven difetents productes, o una que només vol certificar el micropagament i el producte es serveix manualment al comprador.

On si farem incidencia es en la gestió de l'autorització de la venta.
Per gestionar l'autorització d'una venta farem ús d'una **board d'arduino** conectada al xip PN532 perque li proporcioni l'habilitat de comunicarse a través de la tecnologia NFC (Near Field Comunication).  
La tecnologia NFC es un superset de RFID, i es la mateixa tecnologia que utilitzen els smartphones per pagar contactless o les targetes clau dels hotels per obrir les portes.  

### Requirements
Per seguir aquest tutorial es requereixen coneixements bàsics de C++ i d'electrónica.
També farà falta una placa d'[arduino](https://www.arduino.cc/en/Guide) Uno i un xip [PN532](https://www.luisllamas.es/arduino-nfc-pn532/#conexi%C3%B3n-por-spi).  

Per al tutorial recomenem utilitzar una placa oficial d'[Arduino UNO R3](https://docs.arduino.cc/hardware/uno-rev3) i un xip [PN532 d'Elechouse](https://www.elechouse.com/product/pn532-nfc-rfid-module-v4/).  
En el mercat existeixen multiples marques blanques d'arduino, que si be són més barates que les oficials i a priori haurien de funcionar sense cap problema, el cert es que no podem assegurar el correcte funcionament. 
En el cas del xip PN532 pots trobar diferents fabricants al mercat i tots ells haurien de funcionar.

## Hardware
### Arduino
Arduino és una placa petita i intel·ligent que es pot programar per realitzar diferents tasques.  
Aquestes taques poden ser des de fer encendre llums fins a llegir sensors o controlar motors. Arduino és molt utilitzat per principiants i experts en la programació per crear projectes electrònics. Cada board té diferents connectors i components que permeten connectar-hi diferents elements com sensors, servomotors o altres dispositius electrònics.
<div align="center">
  <img src="https://docs.arduino.cc/static/2b141eb1cfe6f465a949c203e4af1b5f/A000066-pinout.png">
</div>

### PN532
A la placa d'Arduino conectarem un xip PN532. Aquest xip ens permetrà crear un canal de comunicació entre la placa d'arduino i qualsevol smartphone que disposi de NFC.  

La tecnologia NFC (Near Field Communication o Comunicació de Camp Proper) és una tecnologia inalàmbrica, com el wifi o el bluetooth, que permet la transferència de dades i la comunicació entre dispositius electrònics que es troben a prop l'un de l'altre, generalment a una distància d'uns pocs centímetres. Funciona mitjançant ones de radiofreqüència de baixa potència.   
Es la mateixa tecnologia que utilitzan els smartphones per fer pagaments contactless.

Per conectar el xip PN532 a la placa d'arduino heu de soldar i conectar els pins seguint el següent diagrama:

<div align="center">
  <img src="https://www.luisllamas.es/wp-content/uploads/2018/04/arduino-nfc-pn532-spi-esquema.png">
</div>

[aqui](https://www.luisllamas.es/arduino-nfc-pn532/#conexi%C3%B3n-por-spi)
- Configurar interface de comunicació SPI  
Podem triar entre tres tipus de interfaces de comunicació NFC en el PN532: I2C, SPI i HSU.  
> A nosaltres ens interesa el SPI.    

Assegurat de configurar el protocol SPI al xip abans de continuar. Ho pots fer movent les pestanyes en aquesta posició en aquesta part del xip:

<div align="center">
  <img src="https://www.luisllamas.es/wp-content/uploads/2018/04/arduino-nfc-pn532-configuracion.jpg">
</div>

---
resaltar posicio SPI al grafic!
=
Seguint el esquema de la imatge de dalt: la pestanya corresponent al pin 1 ha de situar-se a la esquerra i el pin 2 ha de desplaçar-se a la dreta.

> Les pestanyes estan protegides per una membrana que pots retirar amb delicadesa, i després tornar a col·locar per continuar protegint adequadament. 

Tracta sempre el xip amb cura i mou les pestanyes assegurant no malmetre cap element del circuit.

[aqui - link a web luis llamas](https://www.luisllamas.es/arduino-nfc-pn532/#esquema-de-montaje) 

## Software
### Download arduino IDE
Per començar a escriure el nostre codi i poder gestionar els projectes d'arduino primer necesitem preparar el nostre entorn de treball.  
A través del IDE d'arduino (Interface Development Environment) podrem escriure, guardar i descarregar el nostre codi a la placa d'arduino.
Descarrega la versió de IDE adecuada al teu sistema operatiu en [aquesta pàgina.](https://www.arduino.cc/en/Guide)

---
explicacio com funciona Ide?
=

### Download Libraries
Desde la IDE d'arduino ves a Sketch > Include Library > Manage Libraries.
Busca 'pn532' i instala la llibreria 'Adafruit PN532'.

[Oficial guide](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries)

## Code
### Introduction
### Basics
### Emulate tag
### Validation data
### Custom response
## Canut.io certificate
## FAQ
Per al tutorial recomenem utilitzar una placa oficial d'[Arduino UNO R3](https://docs.arduino.cc/hardware/uno-rev3) i un xip [PN532 d'Elechouse](https://www.elechouse.com/product/pn532-nfc-rfid-module-v4/).
En el mercat existeixen multiples marques blanques d'arduino, que si be són més barates que les oficials i a priori haurien de funcionar sense cap problema, el cert es que no podem assegurar el correcte funcionament.  
En el cas del xip PN532 pots trobar diferents fabricants al mercat i tots ells haurien de funcionar.
> Malauradament no podem certificar el correcte funcionament de totes les posibles combinacions dels elements de hardware.
## Links of interest
- [Arduino official website]()
- [Arduino IDE]()
- [About PN532]()
- [NFC indeep]()