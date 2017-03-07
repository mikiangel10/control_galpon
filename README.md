# control_galpon
Control de un galpon con Arduino 

Con este Firmware se busca controlar un galpon de produccion de gallinas ponedoras con los siguientes accesorios:
·sinfin de llenado del comedero(acciona un contactor y se detiene al sensar el comedero lleno con un sensor IR)
·Luces ambientes(mediante LDR y programacion de horarios)
·Ventiladores( se encenderan segun la temperatura ambiente medida por varios ds18x20)
·Sistema de neblina fogger(accionara la bomba intermitentemente segun la Tº medida)
·comunicacion con otros modulos para un control manual o para control redundante mediante nrf 24l01 o similar

Funcionará testeado en Arduino UNO y NANO
