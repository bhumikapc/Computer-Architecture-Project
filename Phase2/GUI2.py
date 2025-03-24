import tkinter as tk
from tkinter import filedialog, Text, Scrollbar
from PIL import Image, ImageTk  # Importing Pillow for image handling
import subprocess # To call cpp file in python

# Initialize the main window (Full-Screen Mode)
root = tk.Tk()
root.title("File Viewer")
root.state("zoomed")  # Full-screen mode
root.configure(bg="#154360")  # Dark Blue Borders

# Main frame with a light blue background inside
border_frame = tk.Frame(root, bg="#D6EAF8", bd=10, relief="solid", padx=20, pady=20)  
border_frame.pack(fill="both", expand=True, padx=30, pady=30)  # Dark blue border effect

# Global variables
uploaded_filename = None  # Stores the uploaded filename

def select_file():
    """Opens file dialog to select and upload a file."""
    global uploaded_filename
    
    file_path = filedialog.askopenfilename(filetypes=[("Assembly Files", "*.mc")])
    if file_path:
        uploaded_filename = file_path.split("/")[-1]  # Extract filename only
        label.config(text=f"Uploaded: {uploaded_filename}")
        run_cpp_simulator(uploaded_filename)

def run_cpp_simulator(uploaded_filename):
    # Define the executable and input file
    executable = "./main_phase2"  # For running main function of main_phase2.cpp
    input_file = uploaded_filename

    # Run the C++ executable with the input file
    subprocess.run([executable, input_file], capture_output=True, text=True, check=True)
    show_file_list()

def display_content(filename):
    """Reads and returns file content or error message."""
    try:
        with open(filename, "r") as f:
            return f.read()
    except FileNotFoundError:
        return "Error: File not found!"

def show_file_list():
    """Displays predefined file content."""
    clear_window()
    
    create_label(f"Uploaded File: {uploaded_filename}", "#85C1E9")
    create_label("Contents of simulation_results.txt:", "#2E86C1", font_size=16)

    content = display_content("simulation_results.txt")
    display_text(content)

    add_navigation_buttons(show_register_files, show_data_files, show_upload_screen)

def show_register_files():
    """Displays contents of Regfile1.txt."""
    clear_window()
    
    create_label(f"Uploaded File: {uploaded_filename}", "#85C1E9")
    create_label("Register File:", "#2E86C1", font_size=16)
    
    content = display_content("register_file.txt")
    display_text(content)

    add_back_button(show_file_list)

def show_data_files():
    """Displays contents of Datafile1.txt."""
    clear_window()
    
    create_label(f"Uploaded File: {uploaded_filename}", "#85C1E9")
    create_label("Data Memory File:", "#2E86C1", font_size=16)
    
    content = display_content("data_memory.txt")
    display_text(content)

    add_back_button(show_file_list)

def show_upload_screen():
    """Initial screen with upload button and image."""
    clear_window()
    
    global label
    label = create_label("No file selected", "#5DADE2", font_size=16)
    
    upload_button = tk.Button(
        border_frame, text="Select & Upload File", font=("Arial", 14, "bold"),
        bg="#2874A6", fg="white", padx=10, pady=5, command=select_file
    )
    upload_button.pack(pady=20)

    # Load and display the image inside a bordered frame
    try:
        img = Image.open("Simulator_image.png")  # Ensure the image is in the same folder
        img = img.resize((900, 500), Image.LANCZOS)  # Resize image
        img = ImageTk.PhotoImage(img)

        img_frame = tk.Frame(border_frame, bg="black", bd=8)  # Black border around image
        img_frame.pack(pady=60)  # Increased padding

        img_label = tk.Label(img_frame, image=img, bg="black")
        img_label.image = img  # Keep reference to avoid garbage collection
        img_label.pack()
    except FileNotFoundError:
        create_label("Image 'Simulator_image.png' not found!", "#E74C3C", font_size=14)

def clear_window():
    """Removes all widgets from the bordered frame."""
    for widget in border_frame.winfo_children():
        widget.destroy()

def add_back_button(back_function):
    """Adds a 'Back' button to navigate to the previous screen."""
    tk.Button(
        border_frame, text="Back", font=("Arial", 14, "bold"),
        bg="#E74C3C", fg="white", padx=10, pady=5, command=back_function
    ).pack(pady=10)

def add_navigation_buttons(reg_func, data_func, back_func):
    """Adds navigation buttons for Register File, Data Memory, and Back."""
    frame = tk.Frame(border_frame, bg="#D5DBDB", pady=10, padx=10)
    frame.pack(pady=10)

    tk.Button(frame, text="Register File", font=("Arial", 14, "bold"),
              bg="#2980B9", fg="white", padx=10, pady=5, command=reg_func).pack(side="left", padx=10)
    
    tk.Button(frame, text="Data Memory", font=("Arial", 14, "bold"),
              bg="#5499C7", fg="white", padx=10, pady=5, command=data_func).pack(side="left", padx=10)
    
    tk.Button(frame, text="Back", font=("Arial", 14, "bold"),
              bg="#E74C3C", fg="white", padx=10, pady=5, command=back_func).pack(side="left", padx=10)

def display_text(content):
    """Displays text content inside a scrollable text widget."""
    frame = tk.Frame(border_frame, bg="#D4E6F1", padx=10, pady=10)
    frame.pack(pady=10, padx=20, fill="both", expand=True)

    text_widget = Text(frame, font=("Arial", 12), wrap="word", height=25, width=80, bg="#EBF5FB")
    text_widget.insert("1.0", content)
    text_widget.config(state="disabled")  # Read-only mode
    text_widget.pack(side="left", fill="both", expand=True)

    scrollbar = Scrollbar(frame, command=text_widget.yview)
    scrollbar.pack(side="right", fill="y")
    text_widget.config(yscrollcommand=scrollbar.set)

def create_label(text, bg_color, font_size=14):
    """Creates a label inside the bordered frame."""
    label = tk.Label(border_frame, text=text, font=("Arial", font_size, "bold"),
                     bg=bg_color, fg="white", padx=10, pady=5)
    label.pack(pady=5, fill="x")
    return label

# Show initial upload screen
show_upload_screen()

# Run the Tkinter event loop
root.mainloop()