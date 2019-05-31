#  O trabalho anterior escolhido foi o de Henrique Matarazo Camillo 
#  Autor:
#   Nome: Henrique Matarazo Camillo
#   NUSP: 10294943
#   Nome: Gyovana Mayara Moriyama
#   NUSP: 10734387

all:
	gcc source/*.c -o programaTrab3 -g 
	@# -Wall -pedantic

run:
	./programaTrab3

clean:
	rm programaTrab3

zip:
	zip -r T3 ./source Makefile

reset:
	unzip -o arquivos-binarios.zip