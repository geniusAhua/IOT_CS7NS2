import com.amazonaws.services.iot.client.AWSIotException;
import com.amazonaws.services.iot.client.AWSIotMqttClient;

public class connecter {
    public void connect() throws AWSIotException {
        SampleUtil.KeyStorePasswordPair pair = SampleUtil.getKeyStorePasswordPair(constant.certificateFile, constant.privateKeyFile);
        AWSIotMqttClient client = new AWSIotMqttClient(constant.clientEndpoint, constant.clientId, pair.keyStore, pair.keyPassword);
        client.connect();
    }

    public static void main(String[] args) throws AWSIotException {
        connecter c = new connecter();
        c.connect();
    }
}
