import os
import time


os.startfile('.\\Debug\\TestGenerator.exe')
time.sleep(5)
os.startfile('.\\Debug\\GiftWrapping.exe')
time.sleep(5)
os.system('python DrawResult/plot_3D4D.py')