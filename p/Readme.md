
## Guia de Uso


1. Para construir la imagen de Docker con la plataforma **mpgt** ejecute la instruccuón:


> docker build -t mpgt .

2. Cree un directorio en ~/docker/ que se llame mpgt, en este directorio almacene todos los archivos que necesite para utilizar la herramienta y utilicelo para guardar los resultados. De no seguir esta recomendación perderá los resultados obtenidos.


3. Para ejecutar el contenedor previamente creado ejecute el scritp mpgt.sh de la siguiente forma:


> ./mpgt.sh


---
## Instalación de NVIDIA Container Toolkit

Se agregan las llaves del repositorio stable de Cuda mediante el commando:

> distribution=$(. /etc/os-release;echo $ID$VERSION_ID) \
   && curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add - \
   && curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | sudo tee /etc/apt/sources.list.d/nvidia-docker.list

Luego se instala el paquete nvidia-docker2 para darle el soporte a docker para que pueda utlizar la targeta gráfica Nvidia:

> sudo apt update && sudo apt install -y nvidia-docker2

Se reinicia el servicio de docker para incluir los cambios hechos:

> sudo systemctl restart docker

Nota: la guia de instalación se obtuvo de [aquí](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)