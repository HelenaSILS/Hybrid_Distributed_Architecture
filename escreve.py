# recebe arquivo conf.
# arquivo de samples
# arquivo de comandos

# conf -> limite T. Tem mais alguma coisa?

# samples -> duas opções:
# SRR23E23E23	X GB
# ou só
# SRR23E23E23

#SRR579868
# do something $.in$$
# then do this $.out
# then this l1_$.final

# -----

# preciso de uma fila da samples em cada noh (afinal, as filas  podem ser compartilhadas)
# # a gente não sabe o tamanho
# tem que fazer um malloc
# pode ser que haja mais variáveis no futuro, melhor usar uma struct
# um dos campos da struct com o nome do sample

# funções:
# lê_arquivo_samples->recebe: arquivo; ponteiro pra scruct; ponteiro para o tamanho da lista/// devolve 0 ou 1
# recebe e abre arquivo
# lê linha e retorna o conteúdo ou "EOF" se tiver acabado
# coloca conteúdo na lista