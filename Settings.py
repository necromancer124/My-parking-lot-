from tkinter import *
from PIL import Image, ImageTk

window = Tk()
window.title("Parking Lot Settings")
window.geometry("500x500")
window.resizable(False, False)

# Window icon (tkinter PhotoImage is fine here)
window_icon = PhotoImage(file='icon.png')
window.iconphoto(True, window_icon)

window.config(background='blue')

# Load image with PIL for resizing
img = Image.open('icon.png')
img = img.resize((25, 25))  # Adjust size here
img_tk = ImageTk.PhotoImage(img)

settingsLabel = Label(
    window,
    text="Settings",
    font=('Arial', 20, 'bold'),
    bg='black',
    fg='#00ff00',
    relief=RAISED,
    bd=10,
    padx=10,
    pady=5,
    image=img_tk,
    compound='left'
)
settingsLabel.pack()
label2 = Label(
    window,
    text="Enter parking rows:",
    font=('Arial', 14),
    bg='blue',
    fg='white'
)
label2.pack()

entry = Entry(window, font=('Arial', 16))
entry.pack(fill='x', padx=20, pady=10)
settingsLabel.image = img_tk

window.mainloop()