# LAZY Corp Operating Systems Project

Welcome to the documentation for the **LAZY Corp Operating Systems Project**! This project showcases three critical implementations designed to enhance efficiency and performance within a simulated operating system environment. The focus is on achieving optimal performance with minimal effort, embodying LAZY Corp’s philosophy of "doing more with less." This README will guide you through the project details, implementation highlights, and usage instructions.

---

## **Table of Contents**

1. [Introduction](#introduction)
2. [Key Features](#key-features)
   - Copy-On-Write Fork
   - Lazy File Manager Simulation
   - Distributed Sorting System
3. [Project Structure](#project-structure)
4. [Setup and Installation](#setup-and-installation)
5. [Usage Instructions](#usage-instructions)
   - Running the COW Fork Implementation
   - Simulating the Lazy File Manager
   - Using the Distributed Sorting System
6. [Performance Insights](#performance-insights)
7. [Conclusion](#conclusion)

---

## **Introduction**

This project implements three primary systems for the LAZY Corp OS:

1. **Copy-On-Write (COW) Fork:** Optimizes memory usage by sharing memory pages between parent and child processes until one process modifies the page.
2. **Lazy File Manager Simulation:** Simulates concurrent file operations (READ, WRITE, DELETE) with constraints on user patience and system resources.
3. **Distributed Sorting System:** Dynamically switches between Count Sort and Merge Sort for efficient file sorting based on file count.

---

## **Key Features**

### **1. Copy-On-Write Fork**

- **Description:** Enhances the traditional `fork` system call by implementing a Copy-On-Write mechanism to minimize memory duplication.
- **Highlights:**
  - Shared pages are marked as read-only initially.
  - Page faults trigger on writes, creating private copies for modification.
  - Significant memory savings and improved process creation efficiency.

### **2. Lazy File Manager Simulation**

- **Description:** Models a file manager processing concurrent requests under realistic constraints.
- **Concurrency Rules:**
  - Multiple READs can occur simultaneously.
  - Only one WRITE per file at a time.
  - DELETE occurs only when no active operations exist.
- **Output:** Provides detailed logs for user requests, cancellations, and system responses.

### **3. Distributed Sorting System**

- **Dynamic Sorting:**
  - Uses **Count Sort** for small file counts (<42 files).
  - Switches to **Merge Sort** for larger datasets.
- **Distributed Implementation:** Tasks are divided across nodes for parallel processing, ensuring efficient utilization of system resources.
- **Output:** Sorted file data based on a user-specified column.

---

## **Project Structure**

```
/
    README.md
    report.pdf
    concurrency/
        Lazy_Read_write/
            lazy_manager.c
        Lazy_sort/
            Makefile
            a.out
            countfile.c
            countfile.h
            distributedsort.c
            mergesort.c
            mergesort.h
            utils.h
    xv6-riscv/
```

---

## **Setup and Installation**

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/shreyashChitkula/LAZY-OS-Concurrency-Sorting-Redefined.git
   cd LAZY-OS-Concurrency-Sorting-Redefined
   ```

2. **Build the Project:**

   - Navigate to the directories with the respective components and use the provided Makefiles.

   - If no Makefiles are there Compile the corresponding C files and run the executables

3. **Build xv6:**

   - Navigate to the `xv6-riscv` directory and compile the modified kernel.

   ```bash
   cd xv6-riscv
   make qemu
   ```

---

## **Usage Instructions**

### **1. Running the COW Fork Implementation**

- Compile and run the `xv6` kernel with the modified COW fork.
- Execute sample programs to test shared memory and page fault behavior.

### **2. Simulating the Lazy File Manager**

- Input format: Provide request details as specified in the problem statement.
- Run the simulation program:

  ```bash
  ./lazy_manager
  ```

- View the detailed output logs in the terminal or output directory.

### **3. Using the Distributed Sorting System**

- Input format: Specify files and sorting criteria as described in the example.
- Execute the sorting program:

  ```bash
  ./distributedsort
  ```

- Sorted results will be displayed in the terminal or saved to the output directory.

---

## **Performance Insights**

### **1. Copy-On-Write Fork**

- **Benefits:**
  - Memory conservation by sharing pages.
  - Improved performance for process creation.
- **Optimization Potential:**
  - Reduce page fault latency.

### **2. Lazy File Manager Simulation**

- **Efficiency:** Balances concurrency with user patience and system limitations.
- **Improvement Areas:** Handling of DELETE operations under high concurrency.

### **3. Distributed Sorting System**

- **Count Sort:** Efficient for small datasets, low memory usage.
- **Merge Sort:** Scales well for larger datasets, higher memory footprint.
- **Execution Times:** Demonstrated scalability with different file counts.

---

## **Conclusion**

This project exemplifies the principles of efficient system design and resource optimization. From the Copy-On-Write fork to the Lazy File Manager and Distributed Sorting System, each component is tailored to handle real-world challenges while adhering to LAZY Corp’s philosophy of maximizing output with minimal input.

Enjoy exploring the world of LAZY Corp’s innovative operating system solutions!

## **Team**

1. Ch.Shreyash
2. Maryala Sai Teja

