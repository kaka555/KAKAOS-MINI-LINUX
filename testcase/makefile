SRCS = testcase21.c

#ifeq ($(PROJECT),stm32f103)
#SRCS += bsp_led.c
#endif

ASM_SRCS = 

$(SUB_TGT): $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SUB_DIR)
	$(LD) $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SUB_DIR:=/$(SUB_TGT)) $(LDFLAG)  -r -o $@

%.o: %.c
	@echo "CC $@"
	@$(CC) $(FLAGS) -c $< -o $@ $(DEF)

%.d :%.c
	@$(CC) $(FLAGS) $< -MM > $@ $(DEF)

%.o :%.s
	@echo "CC $@"
	@$(CC) $(ASFLAGS) -c $< -o $@

sinclude $(SRCS:.c=.d)



$(SUB_DIR) :
	@make -C $@

clean:
	rm -vf $(SUB_TGT) $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SRCS:.c=.d)
	for dir in $(SUB_DIR);do \
	    make -C $$dir clean;\
	done

.PHONY: clean $(SUB_DIR)
