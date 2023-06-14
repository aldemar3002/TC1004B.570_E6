import pyrebase #librería para manejar Firebase con python

#identificadores de nuestra base de datos
config = { 
  "apiKey": "AIzaSyBs8i0oqsu-hM0ffjXYoAdmdu7zj3PxtIo",
  "authDomain": "prueba1-e14aa.firebaseapp.com",
  "databaseURL": "https://prueba1-e14aa-default-rtdb.firebaseio.com",
  "projectId": "prueba1-e14aa",
  "storageBucket": "prueba1-e14aa.appspot.com",
  "messagingSenderId": "576547641472",
  "appId": "1:576547641472:web:fd7ac07a0602164523ae65",
  "measurementId": "G-SLF7S2MT26"
}

#crea la autenticacioon
firebase = pyrebase.initialize_app(config)
#se accesa a la base de datos en Firebase
db = firebase.database()


#Ruta de acceso a los elementos
info = db.child("App_Inventor").get()
print("Leyendo base de datos...")
print()


flag=False; #bool para detectar cambio en usuario y avisar
for item in info.each(): #imprime para cada elemento en App_Inventor

    if(item.key()=="usuario"):
        print(f' {item.key()} = {item.val()}')
    elif(item.key()=="numero"): #lectura de numero
        if(int(item.val())>9):
            db.child("App_Inventor").update({"usuario":"el numero es mayor a 9"}) #cambio en campo usuario
            flag=True;
            print(" El numero es mayor que 9... ")
        else:
            print(f' numero = {item.val()}')    
    elif("temperaturaC" == item.key()): #temperatura en celsius
        print(f' {item.key()} = {item.val()}°C')
    elif("temperaturaF" == item.key()): #temperatura en farhenheit
        print(f' {item.key()} = {item.val()}°F')
    elif("humedad" == item.key()): #nivel de humedad 
        print(f' {item.key()} = {item.val()}%')
    elif("sonido" == item.key()): #booleano de presencia de sonido ambiental
        if(item.val()==False):
            print(" No hay ruidos aparentes")
        else:
            print(" Se escuchó algo")
    elif("flama" == item.key()): #booleano de presencia de flama
        if(item.val()==False):
            print(" No hay presencia de flama")
        else:
            print(" Hay presencia de flama")
    elif("movimiento"==item.key()): #booleano de movimiento
        if(item.val()==False):
            print(" No hay movimiento")
        else:
            print(" Hay movimiento")

print()
if(flag==True):
    print("Campo de usuario se actualizará a 'elemento mayor a 9'") 
    #el cambio se reflejará en el próximo ciclo, 

print("Se han impreso todos los datos de los sensores")