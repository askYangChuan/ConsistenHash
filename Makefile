CC      = gcc

SRCS    = $(wildcard *.c)
OBJS    = $(patsubst %.c, %.o, $(SRCS))
DEPS    = $(patsubst %.c, .%.d, $(SRCS))

%.o : %.c
	@echo -e "  CC\t$@"
	@$(CC) -c $(CFLAGS) $< -o $@
    
.%.d : %.c
	@echo -e "  DEP\t$@"
	@$(CC) -c -MM $(CFLAGS) $< -o $@    

EXEC=main 
       
all:$(DEPS) $(OBJS) $(EXEC)


$(EXEC): $(OBJS)
	@echo -e "  LD\t$@"
	@$(CC) $(OBJS) $(LDFLAGS) -o $@
    

$(clean_dirs):
	@$(MAKE) -C $(patsubst _clean_%, %, $@) clean    
 
clean: $(clean_dirs)    
	@rm -f .*.d *.o *.a *.so $(EXEC)
    
-include $(DEPS)    




