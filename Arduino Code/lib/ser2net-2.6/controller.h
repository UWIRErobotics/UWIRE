#ifndef CONTROLLER
#define CONTROLLER

/* Initialize the controller code, return -1 on error. */
int controller_init(char *controller_port);

struct controller_info;

/* Send some output to a controller port.  The data field is the data
   to write, the count field is the number of bytes to write. */
void controller_output(struct controller_info *cntlr, char *data, int count);

/* Write some data directly to the controllers output port. */
void controller_write(struct controller_info *cntlr, char *data, int count);

#endif /* CONTROLLER */
