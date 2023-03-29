import com.amazonaws.services.iot.client.AWSIotException;
import com.amazonaws.services.iot.client.AWSIotMqttClient;
import com.amazonaws.services.iot.client.AWSIotQos;

public class publish_message {
    void publishMessage(String message) throws AWSIotException {
        SampleUtil.KeyStorePasswordPair pair = SampleUtil.getKeyStorePasswordPair(constant.certificateFile, constant.privateKeyFile);
        AWSIotMqttClient client = new AWSIotMqttClient(constant.clientEndpoint, constant.clientId, pair.keyStore, pair.keyPassword);
        String topic = "my/own/topic";
        String payload = message;
        client.publish(topic, AWSIotQos.QOS0, payload);
    }
    public static void main(String[] args) throws AWSIotException {
        SampleUtil.KeyStorePasswordPair pair = SampleUtil.getKeyStorePasswordPair(constant.certificateFile, constant.privateKeyFile);
        AWSIotMqttClient client = new AWSIotMqttClient(constant.clientEndpoint, constant.clientId, pair.keyStore, pair.keyPassword);
        String topic = "my/own/topic";
        String payload = "payload";
        client.publish(topic, AWSIotQos.QOS0, payload);
    }
}
