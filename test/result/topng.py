import pandas as pd
import matplotlib.pyplot as plt
import os

# pip3 install pandas matplotlib

def plot_benchmark(csv_file, output_dir="plots"):
    df = pd.read_csv(f"{csv_file}.csv")
    df["throughput"] = (
        df["throughput"]
        .astype(str)
        .str.replace(" Mops/sec", "", regex=False)
        .astype(float)
    )
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # 统一 scenario 顺序
    scenario_order = [
        "1P1C",
        "1P4C",
        "1P8C",
        "4P1C",
        "8P1C",
        "2P2C",
        "4P4C",
        "8P8C"
    ]

    df["scenario"] = pd.Categorical(df["scenario"], scenario_order)

    # ----------------------------
    # 1. 每个 scenario 的队列对比
    # ----------------------------
    for scenario in df["scenario"].unique():
        sub = df[df["scenario"] == scenario]

        plt.figure()
        plt.bar(sub["queue"], sub["throughput"])
        plt.xticks(rotation=45)

        plt.ylabel("Throughput (Mops/sec)")
        plt.title(f"Queue comparison - {scenario}")
        plt.tight_layout()

        path = os.path.join(output_dir, f"comparison_{scenario}.png")
        plt.savefig(path)
        plt.close()

    # ----------------------------
    # 2. 每个 queue 的 scaling 图
    # ----------------------------
    for queue in df["queue"].unique():
        sub = df[df["queue"] == queue]

        plt.figure()
        plt.plot(sub["scenario"], sub["throughput"], marker="o")

        plt.ylabel("Throughput (Mops/sec)")
        plt.title(f"Scaling - {queue}")
        plt.tight_layout()

        path = os.path.join(output_dir, f"{csv_file}_{queue}.png")
        plt.savefig(path)
        plt.close()

    # ----------------------------
    # 3. 全部 queue 在同一图
    # ----------------------------
    plt.figure()

    for queue in df["queue"].unique():
        sub = df[df["queue"] == queue]
        plt.plot(sub["scenario"], sub["throughput"], marker="o", label=queue)

    plt.legend()
    plt.ylabel("Throughput (Mops/sec)")
    plt.title("All queues comparison")
    plt.tight_layout()

    path = os.path.join(output_dir, "all_queues.png")
    plt.savefig(path)
    plt.close()

    print(f"Plots saved to: {output_dir}")

plot_benchmark("5000000")
plot_benchmark("50000000")