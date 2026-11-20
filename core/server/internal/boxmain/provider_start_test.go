package boxmain

import (
	"github.com/sagernet/sing-box/adapter"
	"github.com/sagernet/sing/service"
	"testing"
)

// Even configurations without providers need their manager during reference setup.
func TestStartWithoutProviders(t *testing.T) {
	config := []byte(`{"log":{"disabled":true},"outbounds":[{"type":"socks","tag":"proxy","server":"127.0.0.1","server_port":1080,"version":"5"}],"route":{"final":"proxy"}}`)
	if err := Check(config); err != nil {
		t.Fatal(err)
	}
	for i := 0; i < 2; i++ {
		instance, cancel, err := Create(config)
		if err != nil {
			t.Fatal(err)
		}
		manager := service.FromContext[adapter.ProviderManager](instance.Context())
		err = instance.Close()
		cancel()
		if manager == nil {
			t.Fatal("missing provider manager")
		}
		if err != nil {
			t.Fatal(err)
		}
	}
}
