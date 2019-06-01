#  O trabalho anterior escolhido foi o de Henrique Matarazo Camillo 
#  Autor:
#   Nome: Henrique Matarazo Camillo
#   NUSP: 10294943
#   Nome: Gyovana Mayara Moriyama
#   NUSP: 10734387

all:
	gcc source/*.c -o programaTrab4 -g 

run:
	./programaTrab4

clean:
	rm programaTrab4

zip:
	zip -r T4 ./source Makefile

reset:
	unzip -o arquivos-binarios.zip