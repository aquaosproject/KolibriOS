@erase lang.inc
@echo lang fix ru >lang.inc
@fasm scrshoot.asm scrshoot
@kpack scrshoot
@erase lang.inc
@pause