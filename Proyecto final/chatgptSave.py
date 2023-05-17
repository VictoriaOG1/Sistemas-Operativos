import psutil
import matplotlib.pyplot as plt
from datetime import datetime, timedelta

# Variables para almacenar los datos
timestamps = []  # Lista de marcas de tiempo
cpu_usage = []  # Lista de utilización del CPU

# Obtener la marca de tiempo actual
current_time = datetime.now()

# Obtener la marca de tiempo hace una hora
one_hour_ago = current_time - timedelta(hours=1)

# Recopilar los datos de utilización del CPU en el último hora
while current_time > one_hour_ago:
    # Obtener la utilización del CPU
    cpu_percent = psutil.cpu_percent()
    
    # Guardar la marca de tiempo y la utilización del CPU
    timestamps.append(current_time.strftime('%H:%M:%S'))
    cpu_usage.append(cpu_percent)
    
    # Actualizar la marca de tiempo para retroceder un segundo
    current_time -= timedelta(seconds=1)

# Invertir las listas para que estén en orden cronológico ascendente
timestamps = timestamps[::-1]
cpu_usage = cpu_usage[::-1]

# Graficar los datos
plt.plot(timestamps, cpu_usage)
plt.xlabel('Tiempo')
plt.ylabel('Utilización del CPU (%)')
plt.title('Utilización del CPU en la última hora')
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()