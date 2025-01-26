import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import json

# Load the CSV file into a DataFrame
csv_filename = 'sensor_data.csv'
df = pd.read_csv(csv_filename)

# Convert the 'data' column (JSON string) into separate columns
df['data'] = df['data'].apply(lambda x: json.loads(x))

# Expand the 'data' column into separate columns
df = pd.concat([df, df['data'].apply(pd.Series)], axis=1)

# Drop the original 'data' column as it's now expanded
df.drop(columns=['data'], inplace=True)

# Convert the timestamp column to datetime format (remove the 'format' argument)
df['timestamp'] = pd.to_datetime(df['timestamp'])

# Simplify the timestamp to show just hour and minute (or another format)
df['timestamp_simple'] = df['timestamp'].dt.strftime('%H:%M')  # Hour:Minute format

# Set up the plot style
sns.set(style="whitegrid")

# Create subplots for the different parameters
fig, axes = plt.subplots(3, 2, figsize=(12, 10))

# Plot Humidity
sns.lineplot(data=df, x='timestamp_simple', y='humidity', ax=axes[0, 0])
axes[0, 0].set_title('Humidity')

# Plot Temperature
sns.lineplot(data=df, x='timestamp_simple', y='temperature', ax=axes[0, 1])
axes[0, 1].set_title('Temperature')

# Plot Rain
sns.lineplot(data=df, x='timestamp_simple', y='rain', ax=axes[1, 0])
axes[1, 0].set_title('Rain')

# Plot Moisture
sns.lineplot(data=df, x='timestamp_simple', y='moisture', ax=axes[1, 1])
axes[1, 1].set_title('Moisture')

# Plot Water Level
sns.lineplot(data=df, x='timestamp_simple', y='water_level', ax=axes[2, 0])
axes[2, 0].set_title('Water Level')

# Adjust layout
plt.tight_layout()

plt.savefig("sensor_data_visualize.png")

# Show the plot
#plt.show()