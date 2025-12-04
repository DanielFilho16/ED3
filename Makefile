all:
	gcc -o programaTrab main.c funcionalidades.c busca.c utils.c io_arquivos.c csv_parser.c registro_pessoa.c registro_segue.c indice.c validacao.c crud_pessoa.c crud_segue.c grafo.c -lm

run:
	./programaTrab
