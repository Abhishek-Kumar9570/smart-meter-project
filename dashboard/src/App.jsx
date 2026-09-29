import { useEffect, useState, useCallback } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer
} from "recharts";
import "./App.css";

function App() {
  const [analytics, setAnalytics] = useState(null);
  const [history, setHistory] = useState([]);
  const [lastUpdated, setLastUpdated] = useState(new Date());
  const [dataRefreshing, setDataRefreshing] = useState(false);
  const [latestReading, setLatestReading] = useState(null);
  const [forecast, setForecast] = useState(null);
  const [meters, setMeters] = useState([]);
  const [selectedMeter, setSelectedMeter] = useState("M002");
  const [meterConfig, setMeterConfig] = useState(null);
  // Fetch both endpoints — wrapped in useCallback so the reference is stable
  const fetchDashboardData = useCallback(() => {
  Promise.all([
    fetch(
      `http://127.0.0.1:8000/analytics?meterId=${selectedMeter}`
    ).then((response) => response.json()),

    fetch(
      `http://127.0.0.1:8000/power-history?meterId=${selectedMeter}`
    ).then((response) => response.json()),

    fetch(
      `http://127.0.0.1:8000/latest-reading?meterId=${selectedMeter}`
    ).then((response) => response.json()),

    fetch(
      `http://127.0.0.1:8000/forecast?meterId=${selectedMeter}`
    ).then((response) => response.json()),
  ])
    .then(([analyticsData, historyData, latestData, forecastData]) => {
      setAnalytics(analyticsData);
      setHistory(historyData.data);
      setLatestReading(latestData);
      setForecast(forecastData);
    })
    .catch((error) => {
      console.error("Error fetching dashboard data:", error);
    });
}, [selectedMeter]);

   // Update live clock every second
  useEffect(() => {
    const interval = setInterval(() => {
      setLastUpdated(new Date());
    }, 1000);

  return () => clearInterval(interval);
}, []);
  useEffect(() => {
  fetch("http://127.0.0.1:8000/meters")
    .then((response) => response.json())
    .then((data) => {
      setMeters(data.meters);
    })
    .catch((error) => {
      console.error("Error fetching meters:", error);
    });
}, []);
  useEffect(() => {
  fetch(
    `http://127.0.0.1:8000/meter-config?meterId=${selectedMeter}`
  )
    .then((response) => response.json())
    .then((data) => {
      if (data.message) {
        setMeterConfig(null);
        return;
      }

      setMeterConfig(data);
    })
    .catch((error) => {
      console.error(
        "Error fetching meter configuration:",
        error
      );

      setMeterConfig(null);
    });
}, [selectedMeter]);
  // Poll dashboard data every 5 seconds
  useEffect(() => {
    fetchDashboardData(); // initial fetch
    const interval = setInterval(() => {
      fetchDashboardData();
    }, 5000);

    return () => clearInterval(interval);
  }, [fetchDashboardData, selectedMeter]);

  return (
    <div className="dashboard">
      <h1>Smart Energy Meter Dashboard</h1>

      <div className="meter-selector">
        <label htmlFor="meterSelect">Select Meter:</label>

        <select
           id="meterSelect"
           value={selectedMeter}
           onChange={(event) => setSelectedMeter(event.target.value)}
      >
        {meters.map((meter) => (
           <option key={meter} value={meter}>
              {meter}
           </option>
        ))}
        </select>
      </div>

      <p
        className={
          analytics?.status === "ACTIVE"
            ? "live-status active"
            : "live-status offline"
        }
      >
        ●{" "}
        {analytics?.status === "ACTIVE"
            ? "LIVE MONITORING"
            : "METER OFFLINE"}{" "}
          | Last updated: {lastUpdated.toLocaleTimeString()}
          {dataRefreshing ? " | Updating..." : ""}
      </p>

      <div className="meter-info">
        <p>
          <strong>Meter ID:</strong> {analytics?.meterId || "Loading..."}
        </p>

        <p className="meter-status">
          <strong>Status:</strong>{" "}

          <span
             className={
                analytics?.status === "ACTIVE"
                   ? "status-badge active"
                   : "status-badge offline"
             }
        >
             {analytics?.status || "Loading..."}
        </span>
        </p>

        <p>
          <strong>Last Reading:</strong>{" "}
          {latestReading?.timestamp || "Loading..."}
        </p>

        <div
          className={
             analytics?.anomalyStatus === "ANOMALY"
             ? "alert anomaly"
             : "alert normal"
          }
        >
          <p>
             <strong>Alert:</strong>{" "}
             {analytics?.anomalyStatus || "Loading..."}
          </p>

         {analytics?.anomalyStatus === "ANOMALY" && (
          <>
          <p>
            <strong>Type:</strong>{" "}
            {analytics?.anomalyType || "Unknown"}
          </p>

          <p>
            <strong>Reason:</strong>{" "}
            {analytics?.anomalyReason || "No reason available"}
          </p>
        </>
         )}
        </div>
      </div>

      {analytics ? (
        <div className="cards">
          <div className="card">
            <h2>Latest Power</h2>
            <p>{latestReading?.powerW ?? "Loading..."} W</p>
          </div>

          <div className="card forecast-card">
            <h2>Forecast Power</h2>

            <p className="forecast-main">
               {forecast?.forecastPower ?? "Loading..."} W
            </p>

            <div className="forecast-details">
            <p>
              <span>Next Hour</span>
              <strong>
               {forecast?.nextHourKWh ?? "Loading..."} kWh
              </strong>
            </p>

            <p>
              <span>Next 24 Hours</span>
              <strong>
               {forecast?.nextDayKWh ?? "Loading..."} kWh
              </strong>
            </p>
            </div>
          </div>

          <div className="card">
            <h2>Average Power</h2>
            <p>{analytics.averagePower} W</p>
          </div>

          <div className="card">
            <h2>Minimum Power</h2>
            <p>{analytics.minimumPower} W</p>
          </div>

          <div className="card">
            <h2>Maximum Power</h2>
            <p>{analytics.maximumPower} W</p>
          </div>

          <div className="card">
            <h2>Total Energy</h2>
            <p>{analytics.totalEnergy} kWh</p>
          </div>

          <div className="card">
            <h2>Estimated Cost</h2>

            <p>₹{analytics.estimatedCost}</p>

            <small>
               Tariff: ₹{analytics.costPerKWh}/kWh
            </small>
          </div>
        </div>
      ) : (
        <p>Loading analytics...</p>
      )}


      {latestReading && (
        <div className="latest-reading-panel">
          <h2>Latest Reading Details</h2>

          <div className="reading-details">
            <div>
              <span>Pulse Count</span>
              <strong>{latestReading.pulseCount}</strong>
            </div>

            <div>
              <span>Energy Reading</span>
              <strong>{latestReading.energyKWh} kWh</strong>
            </div>

            <div>
              <span>Power Reading</span>
              <strong>{latestReading.powerW} W</strong>
            </div>

            <div>
              <span>Reading Time</span>
              <strong>{latestReading.timestamp}</strong>
            </div>
        </div>
       </div>
      )}



      {meterConfig ? (
       <div className="meter-config-panel">
         <h2>Meter Configuration</h2>

         <div className="config-details">
           <div>
             <span>Meter ID</span>
             <strong>{meterConfig.meterId}</strong>
           </div>

           <div>
             <span>Impulse Constant</span>
             <strong>
               {meterConfig.impulseConstant} pulses/kWh
             </strong>
           </div>

           <div>
             <span>Tariff</span>
             <strong>
               ₹{meterConfig.costPerKWh}/kWh
             </strong>
           </div>

           <div>
             <span>Measurement Interval</span>
             <strong>
                {meterConfig.measurementInterval} seconds
             </strong>
           </div>
          </div>
        </div>
      ) : (
        <div className="meter-config-panel">
          <h2>Meter Configuration</h2>
          <p className="config-unavailable">
             Configuration is not available for {selectedMeter}.
          </p>
        </div>
      )}







      {analytics && (
        <div className="chart-container">
          <h2>
             Live Power Consumption - {selectedMeter}
          </h2>

          <p className="chart-subtitle">
             Showing the latest {Math.min(history.length, 50)} meter readings
          </p>

          <ResponsiveContainer width="100%" height={350}>
            <LineChart data={history.slice(-50)}>
              <CartesianGrid strokeDasharray="3 3" />

              <XAxis
                dataKey="timestamp"
                tickFormatter={(value) => value.substring(11, 19)}
                interval="preserveStartEnd"
                label={{
                  value: "Time",
                  position: "insideBottom",
                  offset: -5
                }}
              />

              <YAxis
                label={{
                  value: "Power (W)",
                  angle: -90,
                  position: "insideLeft"
                }}
              />

              <Tooltip
                 formatter={(value) => [`${value} W`, "Power"]}
                 labelFormatter={(label) => `Time: ${label}`}
                 contentStyle={{
                   backgroundColor: "#ffffff",
                   border: "1px solid #cbd5e1",
                   borderRadius: "8px"
                 }}
              />

              <Line
                type="monotone"
                dataKey="power"
                stroke="#333"
                strokeWidth={3}
              />
            </LineChart>
          </ResponsiveContainer>
        </div>
      )}
    </div>
  );
}

export default App;