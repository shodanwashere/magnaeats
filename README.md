# MagnaEats
**MagnaEats**, a wordplay on Uber Eats™, is a program written as a project for the Operating Systems Curricular Unit of the school year of 2021/2022, Bachelor's in Computer Engineering at the Faculty of Sciences of the University of Lisbon (Faculdade de Ciências da Universidade de Lisboa).
## Authors
- Nuno Dias, fc56330
- Francisco Maia, fc55855
- Bruna Santos, fc56328
## Updates
- **1.0** (April 7th, 2:49 PM) :: Successfully delivers a request to a _Restaurant_, which forwards it to a _Driver_, who delivers it to a _Client_ with buffer size at 10, max ops at 10 and 1 _Restaurant_, 1 _Driver_ and 1 _Client_.
- **1.1** (April 9th, 1:35 AM) :: Bug fixes.
- **1.2** (May 9th, 11:59 AM) :: Updated and added header files for development of stage 2. Implemented `synchronization.c`.
## Building and using
You can go on a big tour and use **gcc** to compile and link everything, but we recommend using `make` and the supplied Makefile to automate this process:
```
$ make
```
Afterwards, just run the program. Be aware that MagnaEats has fancy needs, so you need to specify some stuff:
```
$ ./magnaeats max_ops buffers_size n_restaurants n_drivers n_clients
```
Where:
- `max_ops` is the maximum number of orders that can be created;
- `buffers_size` is the maximum size of the used memory buffers;
- `n_restaurants` is the maximum number of restaurants available;
- `n_drivers` is the maximum number of delivery drivers available;
- `n_clients` is the maximum number of clients available.
### Commands
- `request <client> <restaurant> <dish>` - creates a delivery request for the `restaurant`, made by a `client` asking for a `dish`;
- `status <id>` - checks the status of the order with the id number `id`;
- `stop` - terminates the execution of the **MagnaEats** system;
- `help` - shows the help info for the options above.
