# AWS Example Memory Usage

The following data provides approximate memory footprints for the Realtek AWS examples (based on the `mqtt_mutual_auth` demo).

## RTL8721Dx AWS Memory Usage
### Ameba RTOS v1.2 (CMake build)
AWS usage on `km0_km4_app.bin` : 339.91 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 19,040       | 18.59     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 77,984       | 76.16     | Dynamic allocation area                         |
| **Total**      | **97,024**   | **94.75** | Approximate total memory usage                  |

## RTL8726E / RTL8720E / RTL8713E / RTL8710E AWS Memory Usage
### Ameba RTOS v1.2 (CMake build)
AWS usage on `kr4_km4_app.bin` : 304.19 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 18,976       | 18.53     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 78,080       | 76.25     | Dynamic allocation area                         |
| **Total**      | **97,056**   | **94.78** | Approximate total memory usage                  |

## RTL8730E AWS Memory Usage
### Ameba RTOS v1.2 (CMake build)
AWS usage on `km0_km4_ca32_app.bin` : 452.38 KB

| Memory Region  | Size (Bytes) | Size (KB)  | Notes                                           |
|----------------|--------------|------------|-------------------------------------------------|
| RAM            | 0            | 0          | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 108,992      | 106.44     | Dynamic allocation area                         |
| **Total**      | **108,992**  | **106.44** | Approximate total memory usage                  |
