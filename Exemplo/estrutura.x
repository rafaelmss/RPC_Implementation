program AGENDAPROG
{
  version AGENDAVERS {
    int adicionarTelefoneRemoto(in string,in int) = 0;
    int pesquisarTelefoneRemoto(inout string) = 1;
    int excluirTelefoneRemoto(in string) = 2;
  } = 1;
} = 0x00000001;
