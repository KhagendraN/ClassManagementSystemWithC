# Class Management System

This project is a simple class management system implemented in C. It allows users to create classes, manage subjects, teachers, and students, and record attendance.

## Features

- Create new classes with subjects, teachers, and students.
- Manage attendance for each class.
- Persist class data to a file for future use.

## Getting Started

### Prerequisites

- A C compiler (e.g., GCC)
- Make (for building the project)

### Building the Project

1. Clone the repository:
   ```sh
   git clone <repository_url>
   cd <repository_directory>
   ```

2. Build the project using Make:
   ```sh
   make
   ```

3. Run the compiled program:
   ```sh
   ./main
   ```

### Usage

1. **Create a Class**:
   - Select option 1 from the menu.
   - Follow the prompts to enter class name, number of subjects, teachers, and students.

2. **Manage a Class**:
   - Select option 2 from the menu.
   - Follow the prompts to select a class, enter the password, select a subject and teacher, and enter the date for attendance.

3. **Exit**:
   - Select option 3 from the menu to exit the program.

## File Structure

- `main.c`: The main source file containing the implementation.
- `classes.dat`: Binary file used to persist class data.
- `Makefile`: Makefile for building the project.
- `README.md`: This readme file.
- `.gitignore`: Git ignore file.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project is licensed under the MIT License.

