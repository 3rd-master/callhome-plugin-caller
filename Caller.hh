<?hh //strict
namespace thirdsense\callHome\plugin\caller
{
	use nuclio\core\
	{
		ClassManager,
		plugin\Plugin
	};
	use nuclio\plugin\
	{
		config\Config
	};
	
	<<singleton>>
	class Caller extends Plugin
	{
		private Config $config=Map{};
		
		public static function getInstance(/* HH_FIXME[4033] */...$args):Caller
		{
			$instance=ClassManager::getClassInstance(self::class,...$args);
			return ($instance instanceof self)?$instance:new self(...$args);
		}
		
		public function __construct(Config $config)
		{
			$this->config=$config;
		}
		
		public function sayHi():Map<string,mixed>
		{
			$environment=file_get_contents(ROOT_DIR.'.env') ?? 'develop';
			
			$data=http_build_query
			(
				[
					'appRef'		=>$this->config->get('callHome.key'),
					'version'		=>$this->config->get('application.version'),
					'deviceId'		=>'Server',
					'deviceType'	=>'Server',
					'environment'	=>$environment
				]
			);
			// $result=file_get_contents
			// (
			// 	$this->config->get('callHome.url').'hello',
			// 	false,
			// 	stream_context_create
			// 	(
			// 		[
			// 			'http'=>
			// 			[
			// 				'method'		=>'POST',
			// 				'header'		=>"Content-Type: application/x-www-form-urlencoded\r\n",
			// 								//  ."Content-Length: ".strlen($data)."\r\n",
			// 				'content'		=>$data,
			// 				'ignore_errors'	=>true
			// 			]
			// 		]
			// 	)
			// );
			
			$curl=curl_init();
			curl_setopt($curl, CURLOPT_URL, $this->config->get('callHome.url').'hello');
			curl_setopt($curl, CURLOPT_POST, 1);
			curl_setopt($curl, CURLOPT_POSTFIELDS,$data);
			curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
			$result=curl_exec($curl);
			curl_close($curl);

			$result=new Map(json_decode($result,true));
			if ($result->get('success'))
			{
				return new Map($result->get('data'));
			}
			return Map{};
		}
	}
}