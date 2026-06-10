start:
	@echo "\n----------------------------Makefile Begins----------------------------\n"
	@docker run -it --rm -v $(CURDIR):/home/xv6/xv6-riscv wtakuo/xv6-env \
		-c ' \
		echo "sleep for 2 seconds for fun"; \
		sleep 2; cd xv6-riscv/; \
		echo "----------------------------build xv6 and run it----------------------------"; \
		make qemu; \
		'