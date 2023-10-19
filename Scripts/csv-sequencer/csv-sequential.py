import pandas as pd

# Keep Cols
keep = ['App Name', 'App Id', 'Category', 'Rating', 'Rating Count', 'Free', 'Price', 
        'Released', 'Last Updated', 'Ad Supported', 'In App Purchases']

# Carregar o arquivo CSV original
df = pd.read_csv('..\..\Google-Playstore.csv', usecols=keep, sep=',')

# Função para remover o separador interno
def remove_semicolon(text):
    # Verifique se o valor é uma string
    if isinstance(text, str):
        # Divide o texto em palavras
        words = text.split()
        # Remove ";" de cada palavra
        words = [word.replace(';', '') for word in words]
        # Recria o texto com espaços
        cleaned_text = ' '.join(words)
        return cleaned_text
    else:
        # Se não for uma string, retorne o valor original
        return text


# Aplicar a função para remover ";" à coluna 'App Name'
df['App Name'] = df['App Name'].apply(remove_semicolon)

# Ordene o DataFrame pelo 'App Id'
df_sorted = df.sort_values(by='App Id')

# Salvar o DataFrame ordenado em um novo arquivo CSV
df_sorted.to_csv('..\..\Google-Playstore-Sequential.csv', index=False, sep=';')
