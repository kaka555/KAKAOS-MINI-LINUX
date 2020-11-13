SRCS = stm32f10x_pwr.c stm32f10x_usart.c stm32f10x_rcc.c stm32f10x_gpio.c misc.c

ASM_SRCS = 

CUR_DIR = ${shell pwd}
#SUB_DIR
$(SUB_TGT): $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SUB_DIR)
	$(LD) $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SUB_DIR:=/$(SUB_TGT)) -r -o $@

%.o: %.c
	@$(CC) $(FLAGS) -c $< -o $@ $(DEF)

%.d :%.c
	@$(CC) $(FLAGS) $< -MM > $@ $(DEF)

%.o :%.s
	#@echo "execute .s -> .o"
	@$(CC) $(ASFLAGS) -c $< -o $@

sinclude $(SRCS:.c=.d)



$(SUB_DIR) :
	make -C $@

clean:
	rm -vf $(SUB_TGT) $(SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(SRCS:.c=.d)
	for dir in $(SUB_DIR);do \
	    make -C $$dir clean;\
	done

.PHONY: clean $(SUB_DIR)
