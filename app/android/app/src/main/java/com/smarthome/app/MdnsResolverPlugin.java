package com.smarthome.app;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.util.Log;

import com.getcapacitor.JSObject;
import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.PluginMethod;
import com.getcapacitor.annotation.CapacitorPlugin;

import java.net.InetAddress;

@CapacitorPlugin(name = "MdnsResolver")
public class MdnsResolverPlugin extends Plugin {
    private static final String TAG = "MdnsResolver";
    private NsdManager nsdManager;

    @Override
    public void load() {
        super.load();
        Context context = getContext();
        nsdManager = (NsdManager) context.getSystemService(Context.NSD_SERVICE);
    }

    @PluginMethod
    public void resolveHost(PluginCall call) {
        String hostname = call.getString("hostname");
        
        if (hostname == null || hostname.isEmpty()) {
            call.reject("Hostname is required");
            return;
        }

        Log.d(TAG, "Resolving hostname: " + hostname);

        // If hostname ends with .local, use mDNS resolution
        if (hostname.endsWith(".local")) {
            resolveWithMdns(call, hostname);
        } else {
            // Regular DNS resolution
            resolveWithDns(call, hostname);
        }
    }

    private void resolveWithMdns(PluginCall call, String hostname) {
        // Remove .local suffix for service name
        String serviceName = hostname.replace(".local", "");
        String serviceType = "_workstation._tcp"; // Default workstation service type
        
        // Track if resolution is complete to avoid multiple callbacks
        final boolean[] resolved = {false};
        
        NsdManager.DiscoveryListener discoveryListener = new NsdManager.DiscoveryListener() {
            @Override
            public void onDiscoveryStarted(String regType) {
                Log.d(TAG, "mDNS discovery started");
            }

            @Override
            public void onServiceFound(NsdServiceInfo service) {
                Log.d(TAG, "Service found: " + service.getServiceName());
                if (service.getServiceName().equalsIgnoreCase(serviceName) || 
                    service.getServiceName().toLowerCase().contains(serviceName.toLowerCase())) {
                    
                    nsdManager.resolveService(service, new NsdManager.ResolveListener() {
                        @Override
                        public void onResolveFailed(NsdServiceInfo serviceInfo, int errorCode) {
                            Log.e(TAG, "mDNS resolve failed, error: " + errorCode);
                        }

                        @Override
                        public void onServiceResolved(NsdServiceInfo serviceInfo) {
                            if (!resolved[0]) {
                                resolved[0] = true;
                                InetAddress host = serviceInfo.getHost();
                                if (host != null) {
                                    String ipAddress = host.getHostAddress();
                                    Log.d(TAG, "mDNS resolved " + hostname + " to " + ipAddress);
                                    
                                    JSObject result = new JSObject();
                                    result.put("hostname", hostname);
                                    result.put("ipAddress", ipAddress);
                                    result.put("success", true);
                                    call.resolve(result);
                                } else {
                                    Log.e(TAG, "No host address found");
                                    resolveWithDns(call, hostname);
                                }
                            }
                        }
                    });
                    nsdManager.stopServiceDiscovery(this);
                }
            }

            @Override
            public void onServiceLost(NsdServiceInfo service) {
                Log.d(TAG, "Service lost: " + service.getServiceName());
            }

            @Override
            public void onDiscoveryStopped(String serviceType) {
                Log.d(TAG, "mDNS discovery stopped");
            }

            @Override
            public void onStartDiscoveryFailed(String serviceType, int errorCode) {
                Log.e(TAG, "Discovery start failed: " + errorCode);
                if (!resolved[0]) {
                    resolved[0] = true;
                    resolveWithDns(call, hostname);
                }
            }

            @Override
            public void onStopDiscoveryFailed(String serviceType, int errorCode) {
                Log.e(TAG, "Discovery stop failed: " + errorCode);
            }
        };

        try {
            nsdManager.discoverServices(serviceType, NsdManager.PROTOCOL_DNS_SD, discoveryListener);
            
            // Timeout after 5 seconds - fallback to DNS
            new android.os.Handler().postDelayed(() -> {
                if (!resolved[0]) {
                    resolved[0] = true;
                    Log.w(TAG, "mDNS timeout, falling back to DNS");
                    try {
                        nsdManager.stopServiceDiscovery(discoveryListener);
                    } catch (Exception e) {
                        Log.e(TAG, "Error stopping discovery: " + e.getMessage());
                    }
                    resolveWithDns(call, hostname);
                }
            }, 5000);
        } catch (Exception e) {
            Log.e(TAG, "Error starting discovery: " + e.getMessage());
            if (!resolved[0]) {
                resolved[0] = true;
                resolveWithDns(call, hostname);
            }
        }
    }

    private void resolveWithDns(PluginCall call, String hostname) {
        new Thread(() -> {
            try {
                InetAddress[] addresses = InetAddress.getAllByName(hostname);
                if (addresses != null && addresses.length > 0) {
                    String ipAddress = addresses[0].getHostAddress();
                    Log.d(TAG, "DNS resolved " + hostname + " to " + ipAddress);
                    
                    JSObject result = new JSObject();
                    result.put("hostname", hostname);
                    result.put("ipAddress", ipAddress);
                    result.put("success", true);
                    call.resolve(result);
                } else {
                    call.reject("Could not resolve hostname: " + hostname);
                }
            } catch (Exception e) {
                Log.e(TAG, "DNS resolution failed: " + e.getMessage());
                call.reject("Failed to resolve hostname: " + e.getMessage());
            }
        }).start();
    }
}
