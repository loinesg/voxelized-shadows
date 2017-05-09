using UnityEngine;

public class MeshAnimation : MonoBehaviour 
{
	public Vector3 RotationSpeed;
	public Vector3 TranslationSpeed;
	public float ResetTime = -1f;
	public float StartDelay = 0f;

	private Vector3 _rotation;
	private Vector3 _position;

	private Vector3 _originalRotation;
	private Vector3 _originalPosition;

	private float _timeToStart;
	private float _timeToReset;

	private void Awake()
	{
		_originalPosition = transform.position;
		_originalRotation = transform.eulerAngles;
		_position = transform.position;
		_rotation = transform.localEulerAngles;
		_timeToStart = StartDelay;
		_timeToReset = ResetTime;
	}

	private void Update()
	{
		_timeToStart -= Time.deltaTime;
		if(_timeToStart > 0)
		{
			return;
		}

		// Reset the animation after the given time
		_timeToReset -= Time.deltaTime;
		if(ResetTime > 0 && _timeToReset < 0)
		{
			_position = _originalPosition;
			_rotation = _originalRotation;
			_timeToReset = ResetTime;
		}
		
		// Apply the rotation
		_rotation.x += RotationSpeed.x * Time.deltaTime;
		_rotation.y += RotationSpeed.y * Time.deltaTime;
		_rotation.z += RotationSpeed.z * Time.deltaTime;

		// Update the rotation
		transform.localEulerAngles = _rotation;
		
		// Apply the translation
		_position.x += TranslationSpeed.x * Time.deltaTime;
		_position.y += TranslationSpeed.y * Time.deltaTime;
		_position.z += TranslationSpeed.z * Time.deltaTime;

		// Update the position
		transform.position = _position;
	}
}
