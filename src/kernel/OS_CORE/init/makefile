SRCS = ${shell ls *.c}

ASM_SRCS = 

SUB_DIR_y =

$(SUB_TGT): $(SRCS:.c=.o) $(ASM_SRCS:.S=.o) $(SUB_DIR_y)
	$(LD) $(SRCS:.c=.o) $(ASM_SRCS:.S=.o) $(SUB_DIR_y:=/$(SUB_TGT)) $(LDFLAG)  -r -o $@

%.o: %.c
	@echo "CC $@"
	@$(CC) $(FLAGS) -c $< -o $@ $(DEF)

%.d :%.c
	@$(CC) $(FLAGS) $< -MM > $@ $(DEF)

%.o :%.S
	@echo "CC $@"
	@$(CC) $(ASFLAGS) -c $< -o $@ $(DEF)

sinclude $(SRCS:.c=.d)



$(SUB_DIR_y) :
	@make -C $@

clean:
	rm -vf $(SUB_TGT) $(SRCS:.c=.o) $(ASM_SRCS:.S=.o) $(SRCS:.c=.d)
	for dir in $(SUB_DIR_y);do \
	    make -C $$dir clean;\
	done

.PHONY: clean $(SUB_DIR_y)
